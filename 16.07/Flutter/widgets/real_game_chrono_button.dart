import 'package:flutter/material.dart';
import 'dart:async';

/// Chrono cumulant le temps réel de jeu (actif uniquement pendant le tour d'un joueur).
/// Le chrono démarre/reprend sur `runningNotifier.value = true`
/// Il se met en pause sur `runningNotifier.value = false`
/// Il se réinitialise sur `resetNotifier.value = true`
class RealGameChronoButton extends StatefulWidget {
  final ValueNotifier<bool> runningNotifier; // true = chrono tourne, false = pause
  final ValueNotifier<bool> resetNotifier;   // true = reset chrono

  const RealGameChronoButton({
    required this.runningNotifier,
    required this.resetNotifier,
    super.key,
  });

  @override
  State<RealGameChronoButton> createState() => _RealGameChronoButtonState();
}

class _RealGameChronoButtonState extends State<RealGameChronoButton> {
  late Timer _timer;
  double _elapsed = 0.0;
  double _cumul = 0.0;
  DateTime? _lastStart;
  bool _running = false;

  @override
  void initState() {
    super.initState();
    _timer = Timer.periodic(const Duration(milliseconds: 100), (_) {
      if (_running && _lastStart != null) {
        setState(() {
          _elapsed = _cumul + DateTime.now().difference(_lastStart!).inMilliseconds / 1000.0;
        });
      }
    });

    widget.runningNotifier.addListener(_onRunningChanged);
    widget.resetNotifier.addListener(_onReset);
  }

  void _onRunningChanged() {
    if (widget.runningNotifier.value) {
      // Démarrage ou reprise
      if (!_running) {
        _lastStart = DateTime.now();
        _running = true;
      }
    } else if (_running) {
      // Pause : on ajoute la période à cumul
      if (_lastStart != null) {
        _cumul += DateTime.now().difference(_lastStart!).inMilliseconds / 1000.0;
      }
      _lastStart = null;
      _running = false;
      setState(() {
        _elapsed = _cumul;
      });
    }
  }

  void _onReset() {
    // On remet tout à zéro
    _cumul = 0.0;
    _elapsed = 0.0;
    _lastStart = null;
    _running = false;
    setState(() {});
  }

  String formatChrono(double elapsedSeconds) {
    if (elapsedSeconds < 60) {
      return "${elapsedSeconds.toStringAsFixed(3)} s";
    } else if (elapsedSeconds < 3600) {
      int minutes = (elapsedSeconds ~/ 60);
      double seconds = elapsedSeconds % 60;
      return "${minutes}min ${seconds.toStringAsFixed(3)}s";
    } else {
      int hours = elapsedSeconds ~/ 3600;
      int minutes = ((elapsedSeconds ~/ 60) % 60);
      double seconds = elapsedSeconds % 60;
      return "${hours}h ${minutes}min ${seconds.toStringAsFixed(3)}s";
    }
  }

  @override
  void dispose() {
    _timer.cancel();
    widget.runningNotifier.removeListener(_onRunningChanged);
    widget.resetNotifier.removeListener(_onReset);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return ElevatedButton.icon(
      onPressed: null,
      icon: const Icon(Icons.timer, color: Colors.orange),
      label: Text(
        formatChrono(_elapsed),
        style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.black),
      ),
      style: ElevatedButton.styleFrom(
        backgroundColor: const Color(0xFFFFE082), // jaune clair
        disabledBackgroundColor: const Color(0xFFFFE082),
        elevation: 0,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(20)),
      ),
    );
  }
}
