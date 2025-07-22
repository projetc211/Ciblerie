import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../services/websocket_service.dart';
import '../models/group_model.dart';
import '../models/game_ui_state.dart';
import 'classement_screen.dart';
import '../widgets/countdown_button.dart';
import '../widgets/cumulative_time_button.dart';
import '../widgets/live_clock_button.dart';
import '../widgets/game_chrono_button.dart';

// Fonction utilitaire pour convertir le code groupe en nom lisible
String getModeName(String group) {
  if (group.startsWith('LJ')) return "Le Jeu";
  if (group.startsWith('MS')) return "Mode Survie";
  if (group.startsWith('CPT')) return "Compétition";
  if (group.startsWith('CF')) return "Classic Facile";
  if (group.startsWith('CSP')) return "Classic Semi-Pro";
  if (group.startsWith('CP')) return "Classic Pro";
  if (group.startsWith('CC')) return "Classic Champion";
  if (group.startsWith('CL')) return "Classic Légende";
  return group;
}

class ScoresScreen extends StatefulWidget {
  const ScoresScreen({super.key});

  @override
  State<ScoresScreen> createState() => _ScoresScreenState();
}

class _ScoresScreenState extends State<ScoresScreen> with TickerProviderStateMixin {
  late AnimationController _controller;
  late AnimationController _overlayController;
  late Animation<double> _fadeAnimation;
  late Animation<double> _scaleAnimation;
  late AnimationController _blinkController;
  StreamSubscription<String>? _messageSubscription;

  int currentPlayerIndex = 0;
  bool attente = true;
  bool showFinGameOverlay = false;
  bool hasNavigatedToClassement = false;
  bool isBlinking = false;

  final ValueNotifier<bool> _gameStartedNotifier = ValueNotifier(false);
  final ValueNotifier<bool> _gameEndedNotifier = ValueNotifier(false);

  int getNextPlayerIndex(int idx, int nbJoueurs) {
    return (idx + 1) % nbJoueurs;
  }

  int getNbJoueurs(BuildContext context) {
    final ws = Provider.of<WebSocketService>(context, listen: false);
    final groupModel = Provider.of<GroupModel>(context, listen: false);
    String groupRaw = groupModel.selectedGroup;
    if (groupRaw == '-' || groupRaw.isEmpty) {
      groupRaw = ws.groupNotifier.value;
    }
    final reg = RegExp(r'(\d)$');
    final match = reg.firstMatch(groupRaw);
    if (match != null) {
      final number = int.tryParse(match.group(1)!);
      if (number != null && number >= 1 && number <= 4) {
        return number;
      }
    }
    return 4;
  }

  @override
  void initState() {
    super.initState();

    attente = true;
    _controller = AnimationController(duration: const Duration(milliseconds: 500), vsync: this);
    _fadeAnimation = Tween<double>(begin: 0, end: 1).animate(_controller);
    _scaleAnimation = Tween<double>(begin: 0.95, end: 1).animate(_controller);

    _overlayController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 500),
    );

    _blinkController = AnimationController(
      duration: const Duration(milliseconds: 700),
      vsync: this,
      lowerBound: 0.3,
      upperBound: 1.0,
    );
    _blinkController.value = 1.0;
    _blinkController.addStatusListener((status) {
      if (isBlinking && status == AnimationStatus.completed) {
        _blinkController.reverse();
      } else if (isBlinking && status == AnimationStatus.dismissed) {
        _blinkController.forward();
      }
    });

    _controller.forward();

    WidgetsBinding.instance.addPostFrameCallback((_) {
      _updateBlinkingAnimation();
    });

    _setupWebSocketListener();
  }

  void _setupWebSocketListener() {
    final ws = Provider.of<WebSocketService>(context, listen: false);
    _messageSubscription = ws.messages.listen((message) {
      final data = json.decode(message) as Map<String, dynamic>;
      final type = data['type'] as String?;

      if (type != null) {
        _handleWebSocketMessage(type, message);
      }

      if (type == "game_status") {
        final wsMessage = data['message'] as String?;
        if (wsMessage == "START") {}
        if (wsMessage == "NEXT_PLAYER") {
          final nbJoueurs = getNbJoueurs(context);
          setState(() {
            currentPlayerIndex = getNextPlayerIndex(currentPlayerIndex, nbJoueurs);
            attente = true;
            isBlinking = true;
            _blinkController.forward();
          });
        } else if (wsMessage == "NEXT_TURN") {
          setState(() {
            currentPlayerIndex = 0;
            attente = true;
            isBlinking = false;
            _blinkController.stop();
            _blinkController.value = 1.0;
          });
        } else if (wsMessage == "FIN_GAME") {
          setState(() {
            attente = true;
            isBlinking = false;
            _blinkController.stop();
            _blinkController.value = 1.0;
          });
          _gameEndedNotifier.value = true;
        }
      }
      if (data.containsKey('playerIndex') &&
          data.containsKey('points') &&
          data.containsKey('pointsbonus') &&
          data.containsKey('score')) {
        final playerIndex = data['playerIndex'] as int?;
        if (playerIndex != null) {
          setState(() {
            currentPlayerIndex = playerIndex;
            attente = false;
          });
        }
      }
    });
  }

  void _handleWebSocketMessage(String type, String message) {
    if (!mounted) return;
    final uiState = Provider.of<GameUIState>(context, listen: false);

    setState(() {
      if (type == 'game_status') {
        final data = json.decode(message) as Map<String, dynamic>;
        final wsMessage = data['message'] as String?;

        if (wsMessage == 'START') {
          uiState.updateNextPlayer(
            enabled: false,
            color: Colors.green,
            blinking: false,
          );
          uiState.updateNextTurn(
            enabled: false,
            color: Colors.green,
            blinking: false,
          );
          uiState.updateAttenteGroupe(
            color: Colors.orange,
            enabled: true,
            text: 'Partie en attente',
            blinking: true,
          );
        }
        if (wsMessage == 'NEXT_TURN') {
          uiState.updateNextTurn(
            enabled: true,
            color: Colors.orange,
            blinking: true,
          );
          uiState.updateNextPlayer(
            enabled: false,
            color: Colors.green,
            blinking: false,
          );
          uiState.updateAttenteGroupe(
            color: Colors.green,
            enabled: false,
            text: 'Partie lancée',
            blinking: false,
          );
        }
        if (wsMessage == 'NEXT_PLAYER') {
          uiState.updateNextPlayer(
            enabled: true,
            color: Colors.orange,
            blinking: true,
          );
          uiState.updateNextTurn(
            enabled: false,
            color: Colors.green,
            blinking: false,
          );
          uiState.updateAttenteGroupe(
            color: Colors.green,
            enabled: false,
            text: 'Partie lancée',
            blinking: false,
          );
        }
        if (wsMessage == 'FIN_GAME') {
          _showFinGameOverlay();
          uiState.updateNextPlayer(
            enabled: false,
            color: Colors.blue,
            blinking: false,
          );
          uiState.updateNextTurn(
            enabled: false,
            color: Colors.blue,
            blinking: false,
          );
          uiState.updateAttenteGroupe(
            color: const Color(0xFF7DBFF8),
            enabled: false,
            text: 'Attente groupe',
            blinking: false,
          );
        }
      }
    });

    WidgetsBinding.instance.addPostFrameCallback((_) {
      _updateBlinkingAnimation();
    });
  }

  void _updateBlinkingAnimation() {
    final uiState = Provider.of<GameUIState>(context, listen: false);
    final blinking = uiState.attenteGroupeBlinking || uiState.nextPlayerBlinking || uiState.nextTurnBlinking;
    if (blinking) {
      if (!_blinkController.isAnimating) {
        _blinkController.repeat(reverse: true);
      }
    } else {
      _blinkController.stop();
      _blinkController.value = 1.0;
    }
  }

  void _onStartGamePressed() {
    final ws = Provider.of<WebSocketService>(context, listen: false);
    ws.sendMessage(json.encode({'type': 'game_status', 'message': 'START_GAME'}));
    final uiState = Provider.of<GameUIState>(context, listen: false);
    uiState.updateAttenteGroupe(
      color: Colors.green,
      enabled: false,
      text: 'Partie lancée',
      blinking: false,
      disabledColor: Colors.green,
    );
    _gameStartedNotifier.value = true;
    _updateBlinkingAnimation();
  }

  void _onNextPlayerPressed() {
    final ws = Provider.of<WebSocketService>(context, listen: false);
    ws.sendMessage(json.encode({'type': 'game_status', 'message': 'VALID_E'}));
    final uiState = Provider.of<GameUIState>(context, listen: false);
    uiState.updateNextPlayer(
      enabled: false,
      color: Colors.green,
      blinking: false,
    );
    uiState.updateNextTurn(
      enabled: false,
      color: Colors.green,
      blinking: false,
    );
    setState(() {
      isBlinking = false;
      _blinkController.stop();
      _blinkController.value = 1.0;
    });
    _updateBlinkingAnimation();
  }

  void _onNextTurnPressed() {
    final ws = Provider.of<WebSocketService>(context, listen: false);
    ws.sendMessage(json.encode({'type': 'game_status', 'message': 'VALID_E'}));
    final uiState = Provider.of<GameUIState>(context, listen: false);
    uiState.updateNextPlayer(
      enabled: false,
      color: Colors.green,
      blinking: false,
    );
    uiState.updateNextTurn(
      enabled: false,
      color: Colors.green,
      blinking: false,
    );
    _updateBlinkingAnimation();
  }

  void _showFinGameOverlay() async {
    if (!mounted) return;

    setState(() => showFinGameOverlay = true);
    _overlayController.forward();

    await Future.delayed(const Duration(seconds: 5));
    if (!mounted) return;
    _overlayController.reverse();

    await Future.delayed(const Duration(milliseconds: 500));
    if (mounted && !hasNavigatedToClassement) {
      hasNavigatedToClassement = true;

      final scoresMap = Provider.of<WebSocketService>(context, listen: false).scoresNotifier.value;
      final finalScores = List.generate(4, (i) => scoresMap[i] ?? 0);

      setState(() => showFinGameOverlay = false);

      Provider.of<WebSocketService>(context, listen: false).resetScoresOnly();
      Provider.of<WebSocketService>(context, listen: false).showSaveButton.value = true;

      Navigator.of(context).pushReplacement(
        MaterialPageRoute(
          builder: (_) => ClassementScreen(finalScores: finalScores),
        ),
      );
    }
  }

  @override
  void dispose() {
    _controller.dispose();
    _overlayController.dispose();
    _blinkController.dispose();
    _messageSubscription?.cancel();
    _gameStartedNotifier.dispose();
    _gameEndedNotifier.dispose();
    currentPlayerIndex = 0;
    attente = true;
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final ws = Provider.of<WebSocketService>(context);
    final groupModel = Provider.of<GroupModel>(context);
    final uiState = Provider.of<GameUIState>(context);

    final Map<int, String?> pseudos = ws.pseudonymsNotifier.value;
    _updateBlinkingAnimation();

    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.black,
        title: const Row(
          children: [
            Icon(Icons.bar_chart, color: Colors.white),
            SizedBox(width: 8),
            Text('Scores', style: TextStyle(color: Colors.white)),
          ],
        ),
        actions: [
          const Padding(
            padding: EdgeInsets.symmetric(horizontal: 8.0),
            child: LiveClockButton(),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 8.0),
            child: GameChronoButton(
              gameStartedNotifier: _gameStartedNotifier,
              gameEndedNotifier: _gameEndedNotifier,
            ),
          ),
          ValueListenableBuilder<Map<int, double>>(
            valueListenable: ws.cumulativeTimesNotifier,
            builder: (context, cumulTimes, _) {
              final cumul = attente ? 0.0 : (cumulTimes[currentPlayerIndex] ?? 0.0);
              return CumulativeTimeButton(
                cumulativeNotifier: ValueNotifier(cumul),
              );
            },
          ),
          CountdownButton(countdownNotifier: ws.countdownNotifier),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 8.0),
            child: FadeTransition(
              opacity: uiState.attenteGroupeBlinking
                  ? Tween<double>(begin: 1.0, end: 0.0).animate(_blinkController)
                  : const AlwaysStoppedAnimation(1.0),
              child: ElevatedButton(
                onPressed: uiState.attenteGroupeEnabled ? _onStartGamePressed : null,
                style: ElevatedButton.styleFrom(
                  backgroundColor: uiState.attenteGroupeColor,
                  disabledBackgroundColor: uiState.attenteGroupeDisabledColor,
                  elevation: 0,
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(20),
                  ),
                ),
                child: Text(
                  uiState.attenteGroupeText,
                  style: const TextStyle(
                    fontWeight: FontWeight.bold,
                    color: Colors.black,
                  ),
                ),
              ),
            ),
          ),
          if (uiState.attenteGroupeText != "Attente groupe") ...[
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 8.0),
              child: FadeTransition(
                opacity: uiState.nextPlayerBlinking
                    ? Tween<double>(begin: 1.0, end: 0.0).animate(_blinkController)
                    : const AlwaysStoppedAnimation(1.0),
                child: ElevatedButton(
                  onPressed: uiState.nextPlayerEnabled ? _onNextPlayerPressed : null,
                  style: ElevatedButton.styleFrom(
                    backgroundColor: uiState.nextPlayerColor,
                    disabledBackgroundColor: Colors.blue,
                    elevation: 0,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(20),
                    ),
                  ),
                  child: const Text(
                    'Joueur suivant',
                    style: TextStyle(
                      fontWeight: FontWeight.bold,
                      color: Colors.black,
                    ),
                  ),
                ),
              ),
            ),
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 8.0),
              child: FadeTransition(
                opacity: uiState.nextTurnBlinking
                    ? Tween<double>(begin: 1.0, end: 0.0).animate(_blinkController)
                    : const AlwaysStoppedAnimation(1.0),
                child: ElevatedButton(
                  onPressed: uiState.nextTurnEnabled ? _onNextTurnPressed : null,
                  style: ElevatedButton.styleFrom(
                    backgroundColor: uiState.nextTurnColor,
                    disabledBackgroundColor: Colors.blue,
                    elevation: 0,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(20),
                    ),
                  ),
                  child: const Text(
                    'Tour suivant',
                    style: TextStyle(
                      fontWeight: FontWeight.bold,
                      color: Colors.black,
                    ),
                  ),
                ),
              ),
            ),
          ],
        ],
      ),
      body: SafeArea(
        child: ValueListenableBuilder<Map<int, int>>(
          valueListenable: ws.scoresNotifier,
          builder: (context, scores, _) {
            return LayoutBuilder(
              builder: (context, constraints) {
                final double spacing = constraints.maxWidth * 0.02;
                final double usableHeight = constraints.maxHeight - spacing * 3;
                final double cardHeight = (usableHeight / 2) - spacing;
                final double cardWidth = (constraints.maxWidth - spacing * 3) / 2;
                final double fontSize = cardHeight * 0.4;

                return FadeTransition(
                  opacity: _fadeAnimation,
                  child: ScaleTransition(
                    scale: _scaleAnimation,
                    child: Stack(
                      alignment: Alignment.center,
                      children: [
                        Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Row(
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: [
                                _buildScoreCard(
                                  '${pseudos[0] ?? "J1"} : ${scores[0] ?? 0}',
                                  cardWidth,
                                  cardHeight,
                                  fontSize,
                                  const LinearGradient(
                                    colors: [Color(0xFFAEDCFA), Color(0xFF91C9F9)],
                                  ),
                                  isBlinking && currentPlayerIndex == 0,
                                ),
                                SizedBox(width: spacing),
                                _buildScoreCard(
                                  '${pseudos[1] ?? "J2"} : ${scores[1] ?? 0}',
                                  cardWidth,
                                  cardHeight,
                                  fontSize,
                                  const LinearGradient(
                                    colors: [Color(0xFF91C9F9), Color(0xFF6AB8F7)],
                                  ),
                                  isBlinking && currentPlayerIndex == 1,
                                ),
                              ],
                            ),
                            SizedBox(height: spacing),
                            Row(
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: [
                                _buildScoreCard(
                                  '${pseudos[2] ?? "J3"} : ${scores[2] ?? 0}',
                                  cardWidth,
                                  cardHeight,
                                  fontSize,
                                  const LinearGradient(
                                    colors: [Color(0xFF6AB8F7), Color(0xFF429CF2)],
                                  ),
                                  isBlinking && currentPlayerIndex == 2,
                                ),
                                SizedBox(width: spacing),
                                _buildScoreCard(
                                  '${pseudos[3] ?? "J4"} : ${scores[3] ?? 0}',
                                  cardWidth,
                                  cardHeight,
                                  fontSize,
                                  const LinearGradient(
                                    colors: [Color(0xFF429CF2), Color(0xFF1E88E5)],
                                  ),
                                  isBlinking && currentPlayerIndex == 3,
                                ),
                              ],
                            ),
                          ],
                        ),
                        if (showFinGameOverlay)
                          FadeTransition(
                            opacity: _overlayController,
                            child: Container(
                              width: constraints.maxWidth * 0.75,
                              height: constraints.maxHeight * 0.75,
                              decoration: BoxDecoration(
                                gradient: const LinearGradient(
                                  colors: [Color(0xFFFFE0B2), Color(0xFFFFB74D)],
                                ),
                                borderRadius: BorderRadius.circular(20),
                              ),
                              alignment: Alignment.center,
                              child: FittedBox(
                                fit: BoxFit.scaleDown,
                                child: Text(
                                  'FIN GAME',
                                  style: TextStyle(
                                    fontSize: constraints.maxHeight * 0.15,
                                    fontWeight: FontWeight.bold,
                                    color: Colors.black,
                                  ),
                                ),
                              ),
                            ),
                          ),
                      ],
                    ),
                  ),
                );
              },
            );
          },
        ),
      ),
    );
  }

  Widget _buildScoreCard(
      String text,
      double width,
      double height,
      double fontSize,
      LinearGradient gradient,
      bool blink,
      ) {
    return AnimatedBuilder(
      animation: _blinkController,
      builder: (context, child) {
        return Opacity(
          opacity: blink ? _blinkController.value : 1.0,
          child: Container(
            width: width,
            height: height,
            decoration: BoxDecoration(
              gradient: gradient,
              borderRadius: BorderRadius.circular(16),
              boxShadow: blink
                  ? [
                BoxShadow(
                  color: Colors.yellow.withOpacity(0.7),
                  blurRadius: 18,
                  spreadRadius: 2,
                )
              ]
                  : null,
            ),
            alignment: Alignment.center,
            child: Text(
              text,
              style: TextStyle(
                fontSize: fontSize,
                fontWeight: FontWeight.bold,
                color: Colors.white,
              ),
              textAlign: TextAlign.center,
            ),
          ),
        );
      },
    );
  }
}
