import 'package:flutter/material.dart';
import 'package:flutter/scheduler.dart';
import 'package:provider/provider.dart';
import '../services/score_history_service.dart';
import '../services/websocket_service.dart';
import '../models/group_model.dart';
import 'stats_screen.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'dart:convert';

// Fonction utilitaire pour extraire le niveau à partir du code groupe
String extractLevelFromGroup(String group) {
  if (group.startsWith("CF")) return "Facile";
  if (group.startsWith("CP")) return "Pro";
  if (group.startsWith("CSP")) return "Semi-pro";
  if (group.startsWith("CC")) return "Champion";
  if (group.startsWith("CL")) return "Légende";
  if (group.startsWith("LJ")) return "Le Jeu";
  if (group.startsWith("MS")) return "Mode Survi";
  return group; // fallback
}

class ClassementScreen extends StatefulWidget {
  final List<int>? finalScores;

  const ClassementScreen({super.key, this.finalScores});

  @override
  State<ClassementScreen> createState() => _ClassementScreenState();
}

class _ClassementScreenState extends State<ClassementScreen> with SingleTickerProviderStateMixin {
  late AnimationController _controller;
  late Animation<double> _fadeAnimation;
  late Animation<double> _scaleAnimation;

  final List<List<Color>> gradientColorsList = const [
    [Color(0xFFFFF3E0), Color(0xFFFFCC80)],
    [Color(0xFFFFE0B2), Color(0xFFFFB74D)],
    [Color(0xFFFFCC80), Color(0xFFFFA726)],
    [Color(0xFFFFB74D), Color(0xFFFF8A65)],
    [Color(0xFFFFA726), Color(0xFFFF7043)],
    [Color(0xFFFF8A65), Color(0xFFFF5722)],
  ];

  @override
  void initState() {
    super.initState();
    _controller = AnimationController(duration: const Duration(milliseconds: 500), vsync: this);
    _fadeAnimation = Tween<double>(begin: 0, end: 1).animate(_controller);
    _scaleAnimation = Tween<double>(begin: 0.95, end: 1).animate(_controller);
    SchedulerBinding.instance.addPostFrameCallback((_) => _controller.forward());

    if (widget.finalScores != null) {
      Provider.of<WebSocketService>(context, listen: false).lastFinalScores.value = widget.finalScores;
    }
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  String _formatTime(double seconds) {
    if (seconds == double.infinity) return "--";
    final minutes = (seconds / 60).floor();
    final secs = (seconds % 60).floor();
    return '${minutes}m${secs}s';
  }

  @override
  Widget build(BuildContext context) {
    final ws = Provider.of<WebSocketService>(context);
    final groupModel = Provider.of<GroupModel>(context, listen: false);

    return Scaffold(
      appBar: AppBar(
        title: const Row(
          children: [
            Icon(Icons.emoji_events),
            SizedBox(width: 8),
            Text('Classement'),
          ],
        ),
        actions: [
          ValueListenableBuilder<bool>(
            valueListenable: ws.showSaveButton,
            builder: (context, show, _) {
              if (!show) return const SizedBox.shrink();

              return Padding(
                padding: const EdgeInsets.only(right: 12.0),
                child: ElevatedButton(
                  onPressed: () async {
                    final scores = ws.lastFinalScores.value;
                    final selectedGroup = groupModel.selectedGroup;
                    if (scores != null && scores.isNotEmpty) {
                      final rawPseudos = ws.pseudonymsNotifier.value;
                      final pseudos = <int, String>{
                        for (var i = 0; i < scores.length; i++)
                          i: (rawPseudos[i] != null && rawPseudos[i]!.trim().isNotEmpty)
                              ? rawPseudos[i]!
                              : 'J${i + 1}'
                      };

                      // Charger les meilleurs scores actuels pour ce mode
                      final storage = const FlutterSecureStorage();
                      final groupName = extractLevelFromGroup(selectedGroup);
                      final raw = await storage.read(key: 'player_scores_$groupName');
                      Map<String, dynamic> bestScores = {};
                      if (raw != null) {
                        final decoded = jsonDecode(raw) as Map<String, dynamic>;
                        decoded.forEach((pseudo, entries) {
                          final list = (entries as List).map((e) => Map<String, dynamic>.from(e)).toList();
                          final best = list.fold<int>(0, (max, e) {
                            final score = e['score'];
                            if (score is int) {
                              return score > max ? score : max;
                            }
                            return max;
                          });
                          bestScores[pseudo] = best;
                        });
                      }

                      // Vérifier qui bat son record
                      List<String> lowerScorePlayers = [];
                      for (int i = 0; i < scores.length; i++) {
                        final pseudo = pseudos[i] ?? 'J${i + 1}';
                        final currentScore = scores[i];
                        final best = bestScores[pseudo] ?? 0;
                        if (currentScore <= best) {
                          lowerScorePlayers.add(pseudo);
                        }
                      }

                      // Filtrer uniquement les joueurs ACTIFS (score > 0 dans la partie courante)
                      final List<String> lowerScoreActivePlayers = [];
                      for (int i = 0; i < scores.length; i++) {
                        final pseudo = pseudos[i] ?? 'J${i + 1}';
                        final currentScore = scores[i];
                        if (lowerScorePlayers.contains(pseudo) && currentScore > 0) {
                          lowerScoreActivePlayers.add(pseudo);
                        }
                      }

                      // Pour "score enregistré", ne lister que les pseudos ayant score > 0 dans la partie courante
                      final List<String> activePlayers = [];
                      for (int i = 0; i < scores.length; i++) {
                        final pseudo = pseudos[i] ?? 'J${i + 1}';
                        final currentScore = scores[i];
                        if (currentScore > 0) {
                          activePlayers.add(pseudo);
                        }
                      }

                      String message;
                      if (lowerScoreActivePlayers.isNotEmpty && lowerScoreActivePlayers.length != activePlayers.length) {
                        final enregistres = activePlayers
                            .where((p) => !lowerScoreActivePlayers.contains(p))
                            .join(", ");
                        final nonEnregistres = lowerScoreActivePlayers.join(", ");
                        message =
                        'Score enregistré pour: $enregistres\n'
                            'Score non enregistré pour: $nonEnregistres\n(car inférieur ou égal au meilleur score du classement)';
                      } else if (lowerScoreActivePlayers.length == activePlayers.length && activePlayers.isNotEmpty) {
                        message =
                        'Aucun score enregistré\n'
                            'Tous les scores sont inférieurs ou égaux au meilleur score du classement.';
                      } else if (activePlayers.isNotEmpty) {
                        // Tous enregistrés (tous les joueurs actifs ont été retenus)
                        message = 'Score enregistré pour: ${activePlayers.join(", ")}';
                      } else {
                        message = 'Aucun score actif à enregistrer.';
                      }

                      // Sauvegarde classique
                      await ScoreHistoryService.saveScores(scores, group: selectedGroup, pseudos: pseudos);
                      await ScoreHistoryService.saveScoresForClassementGeneral(
                        scores,
                        pseudos: pseudos,
                        group: groupName,
                      );
                      ws.showSaveButton.value = false;

                      // ----------- RESET SCORES ICI -----------
                      ws.resetScores(); // <-- AJOUT POUR RESET AUTOMATIQUE DES SCORES

                      showDialog(
                        context: context,
                        barrierDismissible: false,
                        builder: (BuildContext dialogContext) {
                          return AlertDialog(
                            title: const Text('Résultat de l\'enregistrement'),
                            content: Text(message),
                            actions: [
                              TextButton(
                                onPressed: () {
                                  Navigator.of(dialogContext).pop();
                                  Navigator.pushAndRemoveUntil(
                                    context,
                                    MaterialPageRoute(builder: (_) => const StatsScreen()),
                                        (route) => route.isFirst,
                                  );
                                },
                                child: const Text('Fermer'),
                              ),
                            ],
                          );
                        },
                      );
                    }
                  },
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.teal,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(20),
                    ),
                  ),
                  child: const Text('Enregistrer la partie'),
                ),
              );
            },
          ),
        ],
      ),
      body: ValueListenableBuilder<List<int>?>(
        valueListenable: ws.lastFinalScores,
        builder: (context, scores, _) {
          if (scores == null || scores.isEmpty) {
            return const Center(
              child: Text(
                'Aucun classement disponible.',
                style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
              ),
            );
          }

          final pseudos = ws.pseudonymsNotifier.value;
          final cumulativeTimes = ws.cumulativeTimesNotifier.value;

          final sortedPlayers = List.generate(scores.length, (index) => {
            'name': pseudos[index] ?? 'J${index + 1}',
            'score': scores[index],
            'time': cumulativeTimes[index] ?? double.infinity,
          })..sort((a, b) {
            final scoreCompare = (b['score'] as int).compareTo(a['score'] as int);
            return scoreCompare != 0 ? scoreCompare :
            (a['time'] as double).compareTo(b['time'] as double);
          });

          final int playerCount = sortedPlayers.length;

          return FadeTransition(
            opacity: _fadeAnimation,
            child: ScaleTransition(
              scale: _scaleAnimation,
              child: LayoutBuilder(
                builder: (context, constraints) {
                  double totalHeight = constraints.maxHeight;
                  double spacing = totalHeight * 0.02;
                  double availableHeight = totalHeight - spacing * (playerCount + 1);
                  double barHeight = availableHeight / (playerCount > 0 ? playerCount : 1);

                  return SingleChildScrollView(
                    child: Padding(
                      padding: EdgeInsets.symmetric(vertical: spacing, horizontal: spacing),
                      child: Column(
                        children: List.generate(playerCount, (index) {
                          final player = sortedPlayers[index];
                          final colors = gradientColorsList[index % gradientColorsList.length];
                          return Padding(
                            padding: EdgeInsets.only(bottom: spacing),
                            child: _buildScoreBar(
                              '${index + 1}. ${player['name']} : ${player['score']} (${_formatTime(player['time'] as double)})',
                              constraints.maxWidth,
                              barHeight,
                              colors,
                            ),
                          );
                        }),
                      ),
                    ),
                  );
                },
              ),
            ),
          );
        },
      ),
    );
  }

  Widget _buildScoreBar(String text, double width, double height, List<Color> gradientColors) {
    return Container(
      width: width,
      height: height,
      decoration: BoxDecoration(
        gradient: LinearGradient(
          colors: gradientColors,
          begin: Alignment.centerLeft,
          end: Alignment.centerRight,
        ),
        borderRadius: BorderRadius.circular(16),
      ),
      alignment: Alignment.center,
      child: FittedBox(
        fit: BoxFit.scaleDown,
        child: Text(
          text,
          style: const TextStyle(
            fontSize: 100,
            fontWeight: FontWeight.bold,
            color: Colors.white,
          ),
        ),
      ),
    );
  }
}
