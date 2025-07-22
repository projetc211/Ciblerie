import 'package:flutter/material.dart';
import 'dart:async';

class GameChronoButton extends StatefulWidget {
  final ValueNotifier<bool> gameStartedNotifier;
  final ValueNotifier<bool> gameEndedNotifier;
  const GameChronoButton({
    required this.gameStartedNotifier,
    required this.gameEndedNotifier,
    super.key
  });
  @override
  State<GameChronoButton> createState() => _GameChronoButtonState();
}

class _GameChronoButtonState extends State<GameChronoButton> {
  late Timer _timer;
  double elapsed = 0.0;
  DateTime? _startTime;
  DateTime? _stopTime;
  bool _running = false;

  @override
  void initState() {
    super.initState();
    _timer = Timer.periodic(const Duration(milliseconds: 100), (_) {
      if (_running && _startTime != null) {
        setState(() {
          elapsed = DateTime.now().difference(_startTime!).inMilliseconds / 1000.0;
        });
      }
    });
    widget.gameStartedNotifier.addListener(_onStartChanged);
    widget.gameEndedNotifier.addListener(_onEndChanged);
  }

  void _onStartChanged() {
    if (widget.gameStartedNotifier.value) {
      setState(() {
        _startTime = DateTime.now();
        _stopTime = null;
        elapsed = 0.0;
        _running = true;
      });
    }
  }

  void _onEndChanged() {
    if (widget.gameEndedNotifier.value) {
      setState(() {
        _stopTime = DateTime.now();
        if (_startTime != null && _stopTime != null) {
          elapsed = _stopTime!.difference(_startTime!).inMilliseconds / 1000.0;
        }
        _running = false;
      });
    }
  }
/*
  String formatChrono(double elapsedSeconds) {
    int hours = elapsedSeconds ~/ 3600;
    int minutes = (elapsedSeconds ~/ 60) % 60;
    int seconds = elapsedSeconds.toInt() % 60;
    int millis = ((elapsedSeconds - elapsedSeconds.floorToDouble()) * 1000).round();
    return "${hours}h ${minutes}min ${seconds}.${millis.toString().padLeft(3, '0')}s";
  }
*/

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
    widget.gameStartedNotifier.removeListener(_onStartChanged);
    widget.gameEndedNotifier.removeListener(_onEndChanged);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return ElevatedButton.icon(
      onPressed: null,
      icon: const Icon(Icons.timer, color: Colors.green),
      label: Text(
        formatChrono(elapsed),
        style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.black),
      ),
      style: ElevatedButton.styleFrom(
        backgroundColor: const Color(0xFFA9F5A9), // vert clair
        disabledBackgroundColor: const Color(0xFFA9F5A9),
        elevation: 0,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(20)),
      ),
    );
  }
}
