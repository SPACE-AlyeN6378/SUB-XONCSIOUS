from mido import MidiFile, tempo2bpm, second2tick

mid = MidiFile("axel-f.mid")
notes = []

tempo = 0

for track in mid.tracks:
    for msg in track:
        if msg.type == "set_tempo":
            tempo = msg.tempo
            

def note_to_bytes(note: int, velocity: int, start: int, end: int) -> bytes:
    # Implementation for converting note to bytes
    note_bytes = note.to_bytes(1, 'big')
    velocity_bytes = velocity.to_bytes(1, 'big')
    start_bytes = start.to_bytes(4, 'big')
    end_bytes = end.to_bytes(4, 'big')

    return note_bytes + velocity_bytes + start_bytes + end_bytes

current_time = 0
active = {}

for msg in mid:
    current_time += msg.time

    if msg.type == "note_on" and msg.velocity > 0:
        active[msg.note] = (current_time, msg.velocity)

    elif msg.type in ("note_off", "note_on") and msg.velocity == 0 or msg.type == "note_off":
        if msg.note in active:
            start, vel = active.pop(msg.note)

            notes.append({
                "note": msg.note,
                # "name": note_name(msg.note),
                "velocity": vel,
                "start": second2tick(start, 96, tempo),
                "end": second2tick(current_time, 96, tempo),
                "duration": second2tick(current_time - start, 96, tempo)
            })

for note in notes[:24]:
    print(note)
# print(mid.ticks_per_beat)