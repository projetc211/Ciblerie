import 'package:flutter/material.dart';
import '../models/grade.dart';
import '../widgets/grade_viewer.dart';
import '../widgets/player_radar_chart.dart';
import '../widgets/player_pie_chart.dart';

class VSInfosScreen extends StatefulWidget {
  const VSInfosScreen({Key? key}) : super(key: key);

  @override
  State<VSInfosScreen> createState() => _VSInfosScreenState();
}

class _VSInfosScreenState extends State<VSInfosScreen> {
  int _vsStars = 2;

  // Exemple d'objet Grade compatible avec ton modèle et grades.json
  final Grade _vsGrade = Grade(
    id: 99,
    name: "VS Démo",
    tier: 1,
    stars: 2,
    maxStars: 5,
    icon: "assets/images/grades/guerrier_1.png", // image fictive, adapte si besoin
    promotionTo: null,
    demotionTo: null,
    demotionRule: "Aucune",
  );

  void _progress() {
    setState(() {
      if (_vsStars < _vsGrade.maxStars) _vsStars++;
    });
  }

  void _regress() {
    setState(() {
      if (_vsStars > 1) _vsStars--;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("Mode VS")),
      backgroundColor: Colors.grey[900],
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: LayoutBuilder(
          builder: (context, constraints) {
            final isSmallScreen = constraints.maxWidth < 700;

            if (isSmallScreen) {
              return Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  GradeViewer(
                    grade: _vsGrade,
                    currentStars: _vsStars,
                    size: 180,
                  ),
                  const SizedBox(height: 24),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      ElevatedButton.icon(
                        onPressed: _regress,
                        icon: const Icon(Icons.arrow_downward),
                        label: const Text("Rétrograder"),
                      ),
                      const SizedBox(width: 16),
                      ElevatedButton.icon(
                        onPressed: _progress,
                        icon: const Icon(Icons.arrow_upward),
                        label: const Text("Monter"),
                      ),
                    ],
                  ),
                  const SizedBox(height: 32),
                  const PlayerRadarChart(),
                  const SizedBox(height: 32),
                  const PlayerPieChart(),
                ],
              );
            }

            // Desktop
            return Row(
              children: [
                Expanded(
                  flex: 1,
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      GradeViewer(
                        grade: _vsGrade,
                        currentStars: _vsStars,
                        size: 240,
                      ),
                      const SizedBox(height: 24),
                      Row(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          SizedBox(
                            width: 120,
                            child: ElevatedButton.icon(
                              onPressed: _regress,
                              icon: const Icon(Icons.arrow_downward),
                              label: const Text("Rétrograder"),
                            ),
                          ),
                          const SizedBox(width: 12),
                          SizedBox(
                            width: 120,
                            child: ElevatedButton.icon(
                              onPressed: _progress,
                              icon: const Icon(Icons.arrow_upward),
                              label: const Text("Monter"),
                            ),
                          ),
                        ],
                      ),
                    ],
                  ),
                ),
                const SizedBox(width: 32),
                Expanded(
                  child: SingleChildScrollView(
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: const [
                        PlayerRadarChart(),
                        SizedBox(height: 20),
                        PlayerPieChart(),
                      ],
                    ),
                  ),
                ),
              ],
            );
          },
        ),
      ),
    );
  }
}
