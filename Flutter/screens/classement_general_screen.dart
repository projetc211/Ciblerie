import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';

const List<String> mainGroups = [
  'Facile',
  'Semi-pro',
  'Pro',
  'Champion',
  'Légende',
  'Le Jeu',
  'Mode Survi',
];

class ClassementGeneralScreen extends StatefulWidget {
  const ClassementGeneralScreen({super.key});
  @override
  State<ClassementGeneralScreen> createState() => _ClassementGeneralScreenState();
}

class _ClassementGeneralScreenState extends State<ClassementGeneralScreen> with TickerProviderStateMixin {
  Map<String, List<Map<String, dynamic>>> _scoresByGroup = {};
  late TabController _tabController;

  final List<List<Color>> gradientColorsList = [
    [Color(0xFF4A148C), Color(0xFF6A1B9A)],
    [Color(0xFF6A1B9A), Color(0xFF8E24AA)],
    [Color(0xFF8E24AA), Color(0xFFBA68C8)],
    [Color(0xFFBA68C8), Color(0xFFCE93D8)],
    [Color(0xFFCE93D8), Color(0xFFD1A0E3)],
    [Color(0xFFD1A0E3), Color(0xFFE8B8F2)],
  ];
  final List<Color> defaultGradient = [Color(0xFFBA68C8), Color(0xFFCE93D8)];

  late AnimationController _controller;
  late Animation<double> _fadeAnimation;
  late Animation<double> _scaleAnimation;

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: mainGroups.length, vsync: this);
    _loadAllScores();

    _controller = AnimationController(
      duration: const Duration(milliseconds: 700),
      vsync: this,
    );
    _fadeAnimation = Tween<double>(begin: 0, end: 1).animate(_controller);
    _scaleAnimation = Tween<double>(begin: 0.95, end: 1).animate(_controller);

    _controller.forward();
  }

  @override
  void dispose() {
    _tabController.dispose();
    _controller.dispose();
    super.dispose();
  }

  Future<void> _loadAllScores() async {
    const storage = FlutterSecureStorage();
    Map<String, List<Map<String, dynamic>>> scoresByGroup = {};
    for (var group in mainGroups) {
      final raw = await storage.read(key: 'player_scores_$group');
      if (raw == null) continue;
      final decoded = jsonDecode(raw) as Map<String, dynamic>;
      Map<String, Map<String, dynamic>> bestByPseudo = {};
      decoded.forEach((player, entries) {
        for (var entry in (entries as List)) {
          final map = Map<String, dynamic>.from(entry);
          final pseudo = map['pseudo'] ?? player;
          final score = map['score'] ?? 0;
          // On ne garde QUE le meilleur score pour chaque pseudo
          if (!bestByPseudo.containsKey(pseudo) || score > bestByPseudo[pseudo]!['score']) {
            bestByPseudo[pseudo] = {
              ...map,
              'pseudo': pseudo,
            };
          }
        }
      });
      // On ne garde QUE les pseudos qui ont au moins un score > 0
      List<Map<String, dynamic>> bestScores = bestByPseudo.values
          .where((entry) => (entry['score'] ?? 0) > 0)
          .toList();
      bestScores.sort((a, b) => (b['score'] as int).compareTo(a['score'] as int));
      scoresByGroup[group] = bestScores;
    }
    setState(() {
      _scoresByGroup = scoresByGroup;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Classement général'),
        bottom: TabBar(
          controller: _tabController,
          isScrollable: true,
          tabs: mainGroups.map((group) => Tab(text: group)).toList(),
        ),
      ),
      body: SafeArea(
        child: FadeTransition(
          opacity: _fadeAnimation,
          child: ScaleTransition(
            scale: _scaleAnimation,
            child: TabBarView(
              controller: _tabController,
              children: mainGroups.map((group) {
                final scores = _scoresByGroup[group] ?? [];
                final displayCount = 6;

                return LayoutBuilder(
                  builder: (context, constraints) {
                    final isMobile = constraints.maxWidth < 700;
                    final double horizontalPadding = isMobile ? 8 : constraints.maxWidth * 0.06;
                    final double verticalPadding = isMobile ? 8 : 18;
                    final double spacing = isMobile ? 6 : constraints.maxHeight * 0.012;
                    final double minCardHeight = isMobile ? 52 : 60;
                    final double maxCardHeight = isMobile ? constraints.maxHeight * 0.15 : constraints.maxHeight * 0.13;
                    final double cardHeight = ((constraints.maxHeight - verticalPadding * 2 - (displayCount - 1) * spacing) / displayCount).clamp(minCardHeight, maxCardHeight);
                    final double fontSize = cardHeight * (isMobile ? 0.40 : 0.48);

                    return ListView.builder(
                      itemCount: displayCount,
                      padding: EdgeInsets.symmetric(
                        vertical: verticalPadding,
                        horizontal: horizontalPadding,
                      ),
                      itemBuilder: (context, index) {
                        String label = '';
                        if (index < scores.length) {
                          final entry = scores[index];
                          label = '${index + 1}${_suffix(index + 1)} : ${entry['pseudo']} (${entry['score']})';
                        } else {
                          label = '${index + 1}${_suffix(index + 1)} :';
                        }
                        final gradient = index < gradientColorsList.length
                            ? gradientColorsList[index]
                            : defaultGradient;

                        return Container(
                          height: cardHeight,
                          margin: EdgeInsets.only(bottom: spacing),
                          decoration: BoxDecoration(
                            gradient: LinearGradient(
                              colors: gradient,
                              begin: Alignment.centerLeft,
                              end: Alignment.centerRight,
                            ),
                            borderRadius: BorderRadius.circular(16),
                            boxShadow: [
                              BoxShadow(
                                color: Colors.black.withOpacity(0.13),
                                blurRadius: 8,
                                offset: const Offset(0, 4),
                              ),
                            ],
                          ),
                          child: Center(
                            child: FittedBox(
                              fit: BoxFit.scaleDown,
                              child: Text(
                                label,
                                style: TextStyle(
                                  fontSize: fontSize,
                                  fontWeight: FontWeight.bold,
                                  color: Colors.black,
                                ),
                                textAlign: TextAlign.center,
                              ),
                            ),
                          ),
                        );
                      },
                    );
                  },
                );
              }).toList(),
            ),
          ),
        ),
      ),
    );
  }

  String _suffix(int rank) => rank == 1 ? 'er' : 'e';
}
