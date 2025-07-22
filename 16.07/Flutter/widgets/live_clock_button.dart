import 'package:flutter/material.dart';
import 'dart:async';

class LiveClockButton extends StatefulWidget {
  const LiveClockButton({super.key});
  @override
  State<LiveClockButton> createState() => _LiveClockButtonState();
}

class _LiveClockButtonState extends State<LiveClockButton> {
  late Timer _timer;
  late DateTime _now;
  @override
  void initState() {
    super.initState();
    _now = DateTime.now();
    _timer = Timer.periodic(const Duration(seconds: 1), (_) {
      setState(() => _now = DateTime.now());
    });
  }

  @override
  void dispose() {
    _timer.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final timeStr =
        "${_now.hour.toString().padLeft(2, '0')}:${_now.minute.toString().padLeft(2, '0')}:${_now.second.toString().padLeft(2, '0')}";
    return ElevatedButton.icon(
      onPressed: null,
      icon: const Icon(Icons.access_time, color: Colors.green),
      label: Text(
        timeStr,
        style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.black),
      ),
      style: ElevatedButton.styleFrom(
        backgroundColor: const Color(0xFFA9F5A9),
        disabledBackgroundColor: const Color(0xFFA9F5A9),
        elevation: 0,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(20)),
      ),
    );
  }
}
