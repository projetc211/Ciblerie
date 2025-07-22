import 'dart:convert';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';

class ScoreHistoryService {
  static final FlutterSecureStorage _storage = const FlutterSecureStorage();
  static const _key = 'score_history';

  /// Sauvegarde les scores avec le groupe courant (utilisé pour Stats)
  static Future<void> saveScores(List<int> scores, {required String group, Map<int, String>? pseudos}) async {
    try {
      final existingData = await _storage.read(key: _key);
      final Map<String, dynamic> currentData =
      existingData != null ? jsonDecode(existingData) : {};

      final String now = DateTime.now().toIso8601String();

      int playerCount = 4;
      final reg = RegExp(r'(\d)$');
      final match = reg.firstMatch(group);
      if (match != null) {
        final number = int.tryParse(match.group(1)!);
        if (number != null && number >= 1 && number <= 4) {
          playerCount = number;
        }
      }

      for (int i = 0; i < playerCount; i++) {
        final playerKey = 'J${i + 1}';
        final entry = {
          'score': scores[i],
          'date': now,
          'group': group,
          if (pseudos != null && pseudos[i] != null) 'pseudo': pseudos[i],
        };

        final List<Map<String, dynamic>> playerScores =
            (currentData[playerKey] as List?)?.map<Map<String, dynamic>>((e) => Map<String, dynamic>.from(e)).toList() ?? [];
        playerScores.add(entry);
        currentData[playerKey] = playerScores;
      }

      await _storage.write(key: _key, value: jsonEncode(currentData));
    } catch (e) {
      print('Erreur dans saveScores : $e');
    }
  }

  static Future<Map<String, List<Map<String, dynamic>>>> loadScores() async {
    try {
      final stored = await _storage.read(key: _key);
      if (stored == null) return {};

      final Map<String, dynamic> decoded = jsonDecode(stored);
      return decoded.map((key, value) {
        final entries = (value as List)
            .map<Map<String, dynamic>>((e) => Map<String, dynamic>.from(e))
            .toList();
        return MapEntry(key, entries);
      });
    } catch (e) {
      print('Erreur dans loadScores : $e');
      return {};
    }
  }

  static Future<void> clearScores() async {
    try {
      await _storage.delete(key: _key);
    } catch (e) {
      print('Erreur dans clearScores : $e');
    }
  }

  /// Sauvegarde les scores pour le classement général (clé 'player_scores_<group>')
  static Future<void> saveScoresForClassementGeneral(
      List<int> scores, {
        required Map<int, String> pseudos,
        required String group,
      }) async {
    try {
      final key = 'player_scores_$group';
      final existing = await _storage.read(key: key);
      final Map<String, dynamic> map =
      existing != null ? jsonDecode(existing) : {};

      final String now = DateTime.now().toIso8601String();
      final reg = RegExp(r'(\d)$');
      int playerCount = 4;
      final match = reg.firstMatch(group);
      if (match != null) {
        final number = int.tryParse(match.group(1)!);
        if (number != null && number >= 1 && number <= 4) {
          playerCount = number;
        }
      }
      for (int i = 0; i < playerCount; i++) {
        final pseudo = (pseudos[i] != null && pseudos[i]!.trim().isNotEmpty)
            ? pseudos[i]!
            : 'J${i + 1}';
        final entry = {
          'score': scores[i],
          'date': now,
          'group': group,
          'pseudo': pseudo,
        };
        final list = (map[pseudo] as List?)?.map((e) => Map<String, dynamic>.from(e)).toList() ?? [];
        list.add(entry);
        map[pseudo] = list;
      }
      // Debug print pour vérification
      print('[DEBUG] Sauvegarde $key = $map');
      await _storage.write(key: key, value: jsonEncode(map));
    } catch (e) {
      print('Erreur dans saveScoresForClassementGeneral : $e');
    }
  }

  static Future<Map<String, List<Map<String, dynamic>>>> loadScoresForMode(String mode) async {
    try {
      final key = 'player_scores_$mode';
      final stored = await _storage.read(key: key);
      if (stored == null) return {};

      final Map<String, dynamic> decoded = jsonDecode(stored);
      return decoded.map((key, value) {
        final entries = (value as List)
            .map<Map<String, dynamic>>((e) => Map<String, dynamic>.from(e))
            .toList();
        return MapEntry(key, entries);
      });
    } catch (e) {
      print('Erreur dans loadScoresForMode : $e');
      return {};
    }
  }

  static Future<void> saveAllForMode(String mode, Map<String, List<Map<String, dynamic>>> data) async {
    try {
      final key = 'player_scores_$mode';
      await _storage.write(key: key, value: jsonEncode(data));
    } catch (e) {
      print('Erreur dans saveAllForMode : $e');
    }
  }

  static Future<void> clearScoresForMode(String mode) async {
    try {
      final key = 'player_scores_$mode';
      await _storage.delete(key: key);
    } catch (e) {
      print('Erreur dans clearScoresForMode : $e');
    }
  }
}
