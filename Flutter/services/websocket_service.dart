import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'package:web_socket_channel/status.dart' as ws_status;

enum ConnectionStatus {
  connected,
  disconnected,
  connecting,
}

enum GameState {
  waitingGroup,
  readyToStart,
  inProgress,
  finished
}

class WebSocketService with ChangeNotifier {
  WebSocketChannel? _channel;
  final StreamController<String> _messageController = StreamController.broadcast();
  final ValueNotifier<Map<int, int>> scoresNotifier = ValueNotifier({0: 0, 1: 0, 2: 0, 3: 0});
  final ValueNotifier<Map<int, Map<int, int>>> frequenciesNotifier = ValueNotifier({
    0: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
    1: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
    2: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
    3: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
  });
  final ValueNotifier<String> groupNotifier = ValueNotifier('-');
  final ValueNotifier<Map<int, String>> pseudonymsNotifier = ValueNotifier({0: 'J1', 1: 'J2', 2: 'J3', 3: 'J4'});
  final ValueNotifier<GameState> gameStateNotifier = ValueNotifier(GameState.waitingGroup);
  final ValueNotifier<List<int>?> lastFinalScores = ValueNotifier(null);
  final ValueNotifier<bool> showSaveButton = ValueNotifier(false);
  final ValueNotifier<Map<int, int>> pointBonusNotifier = ValueNotifier({0: 0, 1: 0, 2: 0, 3: 0});
  final ValueNotifier<double> countdownNotifier = ValueNotifier(0);
  final ValueNotifier<double> cumulativeTimeNotifier = ValueNotifier(0.0);
  final ValueNotifier<Map<int, double>> cumulativeTimesNotifier = ValueNotifier({0: 0.0, 1: 0.0, 2: 0.0, 3: 0.0});
  final ValueNotifier<int> currentPlayerIndexNotifier = ValueNotifier<int>(0);
  final ValueNotifier<double> displayedCumulativeNotifier = ValueNotifier<double>(0.0);

  // Notifier pour le compteur de tirs par joueur
  final ValueNotifier<Map<int, int>> tirCountersNotifier = ValueNotifier({0: 0, 1: 0, 2: 0, 3: 0});

  // Nouveaux notifiers pour les tours et les tirs
  final ValueNotifier<int> tourCourantNotifier = ValueNotifier(1);
  final ValueNotifier<int> nbToursNotifier = ValueNotifier(3);
  final ValueNotifier<int> tirRestantNotifier = ValueNotifier(3);
  final ValueNotifier<int> tirTotalNotifier = ValueNotifier(3);

  int lastPingTime = 0;
  static const int PING_INTERVAL = 30000;
  static const int CLIENT_TIMEOUT = 60000;
  final List<int> lastPongTimes = List.filled(4, 0);

  Completer<bool>? _commandCompleter;
  static const int _maxReconnectAttempts = 5;
  int _reconnectAttempts = 0;

  ConnectionStatus _connectionStatus = ConnectionStatus.disconnected;
  String _serverIp = '';
  Timer? _reconnectTimer;

  bool get isConnected => _connectionStatus == ConnectionStatus.connected;

  WebSocketService() {
    _loadCachedIp();
  }

  Future<void> _loadCachedIp() async {
    final prefs = await SharedPreferences.getInstance();
    _serverIp = prefs.getString('server_ip') ?? '';
    notifyListeners();
  }

  Future<void> connect(String url) async {
    if (url.isEmpty || _connectionStatus == ConnectionStatus.connected) return;

    _updateStatus(ConnectionStatus.connecting);
    _serverIp = url;
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('server_ip', url);

    try {
      final uri = Uri.parse(url);
      _channel = WebSocketChannel.connect(uri);

      await _channel!.ready.timeout(const Duration(seconds: 5));

      _channel!.stream.listen(
        _handleIncomingData,
        onDone: _handleDisconnection,
        onError: (error) {
          debugPrint('WebSocket error: $error');
          _handleDisconnection();
        },
      );

      _updateStatus(ConnectionStatus.connected);
      _resetReconnectAttempts();
      debugPrint('✅ Connexion WebSocket établie avec $url');
    } catch (e) {
      debugPrint('❌ Erreur de connexion WebSocket: $e');
      _handleDisconnection();
    }
  }

  void resetCumulativeTimes() {
    cumulativeTimesNotifier.value = {0: 0.0, 1: 0.0, 2: 0.0, 3: 0.0};
    cumulativeTimeNotifier.value = 0.0;
    displayedCumulativeNotifier.value = 0.0;
    notifyListeners();
  }

  void resetTirCounters() {
    tirCountersNotifier.value = {0: 0, 1: 0, 2: 0, 3: 0};
    debugPrint('🔄 Compteurs de tirs réinitialisés');
    notifyListeners();
  }

  void _handleIncomingData(dynamic data) {
    try {
      if (data is String) {
        debugPrint('📥 Données reçues: $data');
        _messageController.add(data);
        _handleIncomingMessage(data);
        _tryCompleteCommand();
      }
    } catch (e) {
      debugPrint('Erreur de traitement des données: $e');
    }
  }

  void disconnect() {
    try {
      _channel?.sink.close(ws_status.normalClosure);
      debugPrint('🔌 Déconnexion WebSocket intentionnelle');
    } catch (e) {
      debugPrint('Erreur lors de la déconnexion: $e');
    } finally {
      _channel = null;
      _updateStatus(ConnectionStatus.disconnected);
      _cancelReconnectTimer();
    }
  }

  void _handleIncomingMessage(String data) {
    try {
      final decoded = jsonDecode(data) as Map<String, dynamic>;

      if (decoded.containsKey('playerIndex') &&
          decoded.containsKey('points') &&
          decoded.containsKey('pointsbonus') &&
          decoded.containsKey('score')) {
        _handleScoreUpdate(decoded);
      } else if (decoded.containsKey('type') && decoded['type'] == 'pong') {
        _handlePongMessage(decoded);
      } else if (decoded.containsKey('type') && decoded['type'] == 'game_status') {
        final message = decoded['message'] as String?;
        // 1. Récupération du nombre de tirs sur COUNTDOWN et END_MANCHE
        if ((message == 'COUNTDOWN' || message == 'END_MANCHE') && decoded.containsKey('playerIndex')) {
          final int playerIndex = decoded['playerIndex'] as int;
          final current = Map<int, int>.from(tirCountersNotifier.value);
          current[playerIndex] = (current[playerIndex] ?? 0) + 1;
          tirCountersNotifier.value = current;
          debugPrint('🔫 Mise à jour tirs joueur $playerIndex: ${current[playerIndex]}');
        }
        // 2. Gestion du countdown (pour l'affichage du timer)
        if (message == 'COUNTDOWN' && decoded.containsKey('value')) {
          countdownNotifier.value = (decoded['value'] as num).toDouble();
          notifyListeners();
        }
        // 3. Gestion du cumul de temps utilisé par joueur
        else if (message == 'END_MANCHE' && decoded.containsKey('value') && decoded.containsKey('playerIndex')) {
          final playerIndex = decoded['playerIndex'] as int;
          final tempsUtilise = (decoded['value'] as num).toDouble();
          final cumulTimes = Map<int, double>.from(cumulativeTimesNotifier.value);
          cumulTimes[playerIndex] = (cumulTimes[playerIndex] ?? 0.0) + tempsUtilise;
          cumulativeTimesNotifier.value = cumulTimes;
          debugPrint("[CUMUL] Joueur $playerIndex: +${tempsUtilise}s → Total: ${cumulTimes[playerIndex]}s");
        } else {
          _handleGameStatus(message);
          if (message == "NEXT_PLAYER") {
            int nbJoueurs = 4;
            final reg = RegExp(r'(\d)$');
            final match = reg.firstMatch(groupNotifier.value);
            if (match != null) {
              final number = int.tryParse(match.group(1)!);
              if (number != null && number >= 1 && number <= 4) {
                nbJoueurs = number;
              }
            }
            int nextIndex = (currentPlayerIndexNotifier.value + 1) % nbJoueurs;
            currentPlayerIndexNotifier.value = nextIndex;
            displayedCumulativeNotifier.value = 0.0;
          } else if (message == "NEXT_TURN") {
            currentPlayerIndexNotifier.value = 0;
            displayedCumulativeNotifier.value = 0.0;
          }
        }
      } else {
        debugPrint('⚠️ Unrecognized message format: $data');
      }
    } catch (e) {
      debugPrint('❌ Error decoding message: $e');
    }
  }

  void _handleGameStatus(String? status) {
    debugPrint('🔄 Traitement du statut: $status');
    switch (status) {
      case 'waiting_group':
        gameStateNotifier.value = GameState.waitingGroup;
        break;
      case 'ready_to_start':
        gameStateNotifier.value = GameState.readyToStart;
        break;
      case 'in_progress':
        gameStateNotifier.value = GameState.inProgress;
        break;
      case 'finished':
        gameStateNotifier.value = GameState.finished;
        break;
    }
    notifyListeners();
  }

  void _handleScoreUpdate(Map<String, dynamic> data) {
    try {
      final playerIndex = data['playerIndex'] as int;
      final points = data['points'] as int;
      final pointsbonus = data['pointsbonus'] as int;
      final score = data['score'] as int;

      final scores = Map<int, int>.from(scoresNotifier.value);
      final frequencies = Map<int, Map<int, int>>.from(frequenciesNotifier.value);
      final pointBonus = Map<int, int>.from(pointBonusNotifier.value);

      scores[playerIndex] = score;
      scoresNotifier.value = scores;

      frequencies[playerIndex] = Map<int, int>.from(frequencies[playerIndex] ?? {});
      frequencies[playerIndex]![points] = (frequencies[playerIndex]![points] ?? 0) + 1;
      frequenciesNotifier.value = frequencies;

      pointBonus[playerIndex] = pointsbonus;
      pointBonusNotifier.value = pointBonus;

      debugPrint('📊 Mise à jour score joueur ${playerIndex}: points=$points, bonus=$pointsbonus, score=$score');
      notifyListeners();

      currentPlayerIndexNotifier.value = playerIndex;
      displayedCumulativeNotifier.value = cumulativeTimesNotifier.value[playerIndex] ?? 0.0;

    } catch (e) {
      debugPrint('❌ Erreur mise à jour score: $e');
    }
  }

  void _handlePongMessage(Map<String, dynamic> data) {
    final playerIndex = data['player'] as int?;
    if (playerIndex != null && playerIndex >= 0 && playerIndex < 4) {
      lastPongTimes[playerIndex] = DateTime.now().millisecondsSinceEpoch;
    }
  }

  Future<bool> sendCommand(String type, String message, {Duration? timeout}) async {
    if (_channel == null || !isConnected) {
      debugPrint('⚠️ Impossible d\'envoyer la commande - Non connecté');
      return false;
    }

    _commandCompleter = Completer<bool>();
    final timer = timeout != null ? Timer(timeout, () {
      if (!_commandCompleter!.isCompleted) {
        _commandCompleter!.complete(false);
        debugPrint('⏱️ Timeout commande $type');
      }
    }) : null;

    try {
      final command = jsonEncode({'type': type, 'message': message});
      _channel!.sink.add(command);
      debugPrint('📤 Commande envoyée: $command');

      return await _commandCompleter!.future.then((value) {
        timer?.cancel();
        return value;
      }).catchError((e) {
        debugPrint('❌ Erreur commande: $e');
        return false;
      });
    } catch (e) {
      debugPrint('❌ Erreur envoi commande: $e');
      timer?.cancel();
      return false;
    }
  }

  void _tryCompleteCommand() {
    if (_commandCompleter != null && !_commandCompleter!.isCompleted) {
      _commandCompleter!.complete(true);
      _commandCompleter = null;
      debugPrint('✅ Commande confirmée');
    }
  }

  void _handleDisconnection() {
    if (_connectionStatus != ConnectionStatus.disconnected) {
      debugPrint('🔌 Déconnexion WebSocket détectée');
      _updateStatus(ConnectionStatus.disconnected);

      if (_reconnectAttempts < _maxReconnectAttempts && _serverIp.isNotEmpty) {
        _reconnectAttempts++;
        final delay = Duration(seconds: _reconnectAttempts * 2);
        debugPrint('♻️ Tentative de reconnexion #$_reconnectAttempts dans ${delay.inSeconds}s');
        _startReconnectTimer(delay);
      }
    }
  }

  void _startReconnectTimer([Duration delay = const Duration(seconds: 5)]) {
    _reconnectTimer?.cancel();
    _reconnectTimer = Timer(delay, () {
      if (_connectionStatus != ConnectionStatus.connected && _serverIp.isNotEmpty) {
        debugPrint('🔁 Tentative de reconnexion...');
        connect(_serverIp);
      }
    });
  }

  void _resetReconnectAttempts() {
    _reconnectAttempts = 0;
    _cancelReconnectTimer();
  }

  void _cancelReconnectTimer() {
    _reconnectTimer?.cancel();
    _reconnectTimer = null;
  }

  void _updateStatus(ConnectionStatus newStatus) {
    if (_connectionStatus != newStatus) {
      _connectionStatus = newStatus;
      debugPrint('🔄 Statut connexion: $newStatus');
      notifyListeners();
    }
  }

  void sendMessage(String message) {
    if (_channel != null && isConnected) {
      _channel!.sink.add(message);
    }
  }

  void setGameState(GameState state) {
    gameStateNotifier.value = state;
    debugPrint('🎮 Changement état jeu: $state');
    notifyListeners();
  }

  Stream<String> get messages => _messageController.stream;
  ConnectionStatus get status => _connectionStatus;
  String get serverIp => _serverIp;

  void sendPing() {
    if (isConnected) {
      try {
        final now = DateTime.now().millisecondsSinceEpoch;
        if (now - lastPingTime > PING_INTERVAL) {
          _channel!.sink.add(jsonEncode({'type': 'ping', 'time': now}));
          lastPingTime = now;
          debugPrint('🏓 Ping envoyé');
        }
      } catch (e) {
        debugPrint('❌ Erreur envoi ping: $e');
        _handleDisconnection();
      }
    }
  }

  @override
  void dispose() {
    debugPrint('♻️ Disposing WebSocketService');
    disconnect();
    _messageController.close();
    scoresNotifier.dispose();
    frequenciesNotifier.dispose();
    groupNotifier.dispose();
    pseudonymsNotifier.dispose();
    gameStateNotifier.dispose();
    lastFinalScores.dispose();
    showSaveButton.dispose();
    pointBonusNotifier.dispose();
    countdownNotifier.dispose();
    cumulativeTimeNotifier.dispose();
    cumulativeTimesNotifier.dispose();
    currentPlayerIndexNotifier.dispose();
    displayedCumulativeNotifier.dispose();
    tirCountersNotifier.dispose();
    _cancelReconnectTimer();
    super.dispose();
  }

  void setPseudonyms(Map<int, String> pseudonyms) {
    pseudonymsNotifier.value = Map<int, String>.from(pseudonyms);
    debugPrint('🏷️ Pseudonymes mis à jour: $pseudonyms');
    notifyListeners();
  }

  Future<void> setServerIp(String ip) async {
    if (!_validateIp(ip)) {
      debugPrint('❌ IP invalide: $ip');
      return;
    }

    _serverIp = ip;
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('server_ip', ip);
    debugPrint('📌 IP serveur mise à jour: $ip');
    notifyListeners();
  }

  bool _validateIp(String ip) {
    return RegExp(r'^\d{1,3}(\.\d{1,3}){3}(:\d+)?$').hasMatch(ip);
  }

  void resetAllScores() {
    scoresNotifier.value = {0: 0, 1: 0, 2: 0, 3: 0};
    frequenciesNotifier.value = {
      0: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
      1: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
      2: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
      3: {0: 0, 5: 0, 10: 0, 15: 0, 25: 0, 50: 0, 100: 0, 150: 0, 200: 0, 250: 0},
    };
    pointBonusNotifier.value = {0: 0, 1: 0, 2: 0, 3: 0};
    debugPrint('🔄 Tous les scores réinitialisés');
    notifyListeners();
  }

  void resetScoresOnly() {
    scoresNotifier.value = {0: 0, 1: 0, 2: 0, 3: 0};
    pointBonusNotifier.value = {0: 0, 1: 0, 2: 0, 3: 0};
    notifyListeners();
  }

  Map<int, int> _scores = {0: 0, 1: 0, 2: 0, 3: 0};
  Map<int, int> get scores => _scores;
  void resetScores() {
    _scores = {0: 0, 1: 0, 2: 0, 3: 0};
    notifyListeners();
  }
}
