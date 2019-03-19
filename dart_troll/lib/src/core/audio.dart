import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_troll/src/proto/action.pb.dart';

/// Play a music track from resources identified by [trackId].
void playMusic(String trackId, {int repeat = 1}) {
  final action = Action()
    ..playAudio = (SfxAction()
      ..trackId = trackId
      ..repeat = repeat);
  troll.execute(action.writeToBuffer());
}

/// Stop the current music track.
void stopMusic() {
  final action = Action()..stopAudio = (SfxAction()..trackId = '');
  troll.execute(action.writeToBuffer());
}

/// Pause the current music track.
void pauseMusic() {
  final action = Action()..pauseAudio = (SfxAction()..trackId = '');
  troll.execute(action.writeToBuffer());
}

/// Resume playing the music track that was previously paused.
void resumeMusic() {
  final action = Action()..resumeAudio = (SfxAction()..trackId = '');
  troll.execute(action.writeToBuffer());
}

/// Play a sound effect from resources identified by [sfxId].
void playSfx(String sfxId, {int repeat = 1}) {
  final action = Action()
    ..playAudio = (SfxAction()
      ..sfxId = sfxId
      ..repeat = repeat);
  troll.execute(action.writeToBuffer());
}

/// Stop the [sfxId] sound effect currently playing.
void stopSfx(String sfxId) {
  final action = Action()..stopAudio = (SfxAction()..sfxId = sfxId);
  troll.execute(action.writeToBuffer());
}

/// Pause the [sfxId] sound effect currently playing.
void pauseSfx(String sfxId) {
  final action = Action()..pauseAudio = (SfxAction()..sfxId = sfxId);
  troll.execute(action.writeToBuffer());
}

/// Resume [sfxId] sound effect that was previously paused.
void resumeSfx(String sfxId) {
  final action = Action()..resumeAudio = (SfxAction()..sfxId = sfxId);
  troll.execute(action.writeToBuffer());
}
