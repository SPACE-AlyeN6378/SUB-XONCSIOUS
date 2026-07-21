def padding(list_of_words: list[str]):
    max_word_length = max([len(word) for word in list_of_words])
    
    new_list = []
    for word in list_of_words:
        space_length = max_word_length - len(word)
        new_list.append(space_length*" " + word)
        
    return new_list


NOTES_SHARP = ['C', 'Cs', 'D', 'Ds', 'E', 'F', 'Fs', 'G', 'Gs', 'A', 'As', 'B']
NOTES_FLAT  = ['C', 'Db', 'D', 'Eb', 'E', 'F', 'Gb', 'G', 'Ab', 'A', 'Bb', 'B']

notes = [note + '_1' for note in NOTES_SHARP]
notes_ = [note + '_1' for note in NOTES_FLAT]

for i in range(10):
    notes.extend([f'{note}{i}' for note in NOTES_SHARP])
    notes_.extend([f'{note}{i}' for note in NOTES_FLAT])

notes = padding(notes[:128])
notes_ = padding(notes_[:128])

midi_notes = {note: number for number, note in enumerate(notes)}
midi_notes.update(
    {note: number for number, note in enumerate(notes_)}
)

midi_notes = dict(sorted(midi_notes.items(), key=lambda x: x[1]))

with open("MidiNotes_temp.h", 'r') as file:
    code = file.read()
    
with open("MidiNotes.h", 'w') as file:
    file.write(code.replace("// midi_notes", "\n".join([f"\tinline constexpr unsigned char {name} = {number:3d};" for name, number in midi_notes.items()])))
    
print(code)
