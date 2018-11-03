import pytroll.actions
import troll


def PlayMusic(track_id, repeat=1):
    action = pytroll.actions.PlayAudio(track_id=track_id, repeat=repeat)
    troll.execute(action.SerializeToString())


def StopMusic():
    action = pytroll.actions.StopAudio()
    troll.execute(action.SerializeToString())


def PauseMusic():
    action = pytroll.actions.PauseAudio()
    troll.execute(action.SerializeToString())


def ResumeMusic():
    action = pytroll.actions.ResumeAudio()
    troll.execute(action.SerializeToString())


def PlaySfx(sfx_id, repeat=1):
    action = pytroll.actions.PlayAudio(sfx_id=sfx_id, repeat=repeat)
    troll.execute(action.SerializeToString())


def StopSfx(sfx_id):
    action = pytroll.actions.StopAudio(sfx_id=sfx_id)
    troll.execute(action.SerializeToString())


def PauseSfx(sfx_id):
    action = pytroll.actions.PauseAudio(sfx_id=sfx_id)
    troll.execute(action.SerializeToString())


def ResumeSfx(sfx_id):
    action = pytroll.actions.ResumeAudio(sfx_id=sfx_id)
    troll.execute(action.SerializeToString())
