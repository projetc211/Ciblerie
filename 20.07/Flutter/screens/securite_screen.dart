import 'package:flutter/material.dart';
import '../widgets/animated_scaffold.dart';

class SecuriteScreen extends StatelessWidget {
  final ValueNotifier<Map<int, int>> tirCountersNotifier;
  const SecuriteScreen({super.key, required this.tirCountersNotifier});

  @override
  Widget build(BuildContext context) {
    return AnimatedScaffold(
      appBar: AppBar(
        title: const Text('Sécurité'),
      ),
      child: Center(
        child: Padding(
          padding: const EdgeInsets.all(24.0),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              const Text(
                "Compteur de tirs par joueur",
                style: TextStyle(
                  fontSize: 24,
                  fontWeight: FontWeight.bold,
                ),
              ),
              const SizedBox(height: 24),
              ValueListenableBuilder<Map<int, int>>(
                valueListenable: tirCountersNotifier,
                builder: (context, tirCounters, _) {
                  return Column(
                    children: List.generate(4, (i) {
                      return Card(
                        margin: const EdgeInsets.symmetric(vertical: 8, horizontal: 0),
                        child: ListTile(
                          leading: CircleAvatar(child: Text("J${i+1}")),
                          title: Text("Nombre de tirs"),
                          trailing: Text("${tirCounters[i] ?? 0}", style: const TextStyle(fontSize: 20)),
                        ),
                      );
                    }),
                  );
                },
              ),
            ],
          ),
        ),
      ),
    );
  }
}
