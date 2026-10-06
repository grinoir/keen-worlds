"""Krisle speaks: the talks and endings of Atroxian Realm (Gridlock's game)
as voice clips (Keen Worlds).

    python3 tools/kw_arealm_voice.py omnispeak/data/voice

The words are the game's own (its text chunks, shown page by page as its
story screens are). Keen and the narrator keep their voices; new here:

  Atroxus                          am_fenrir   lowered, and made a machine's
  Elder Cyraanae-Augustrus Krislen bm_george   lowered a little
  Cyraaun Kriuus-Llquo             af_kore     the Krile who meets Keen at the
                                               crash, in the crystal shrine and
                                               at the end

(The first two are voices other characters have in other worlds, changed:
every male voice Kokoro has is in use.)

A clip is AR_<SCREEN>_<page>.RAW, where SCREEN names one of the game's text
screens: BWB (the crash site), ELDER (the Great Ziggurat), ATROXUS (the Sky
Sanctum), SHRINE (the secret crystal shrine), BOX1 to BOX3 (Atroxus at the
generators), END (after the fight), END2 (the Realm Reactor) and NORMALEND.
Its last pages (what to look for, the author's note) are silent, and so is
Atroxus's dying gibberish.

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN, NARRATOR

ATROXUS, ELDER, KRILE = 'am_fenrir', 'bm_george', 'af_kore'
STYLE = dict(BASE_STYLE)
STYLE[ATROXUS] = dict(pitch=0.88)
STYLE[ELDER] = dict(pitch=0.94)
ROBOT = ("afftfilt=real='hypot(re,im)*sin(0)':imag='hypot(re,im)*cos(0)':win_size=512:overlap=0.75,"
         "volume=1.5,alimiter=limit=0.95")
EFFECTS = {ATROXUS: ROBOT}
GAP = 0.3

K, N, X, E, L = KEEN, NARRATOR, ATROXUS, ELDER, KRILE
CLIPS = {
    # ---- the crash site: Keen and the Krile
    'AR_BWB_00.RAW': [(L, ["Commander, we are honored.", "If the Atroxians are not stopped, my people will surely perish.",
                           "Only you can end the exile of the Krile."])],
    'AR_BWB_01.RAW': [(K, ["I promise you, I will not let that happen.", "Atroxus and the rest of them are toast!"]),
                      (L, ["You really are as amazing as the galactic rumors tell.",
                           "But, you will have to face greater evil than ever before.",
                           "I have seen terrible things in these past days..."])],
    'AR_BWB_02.RAW': [(K, ["Don't you worry about me.", "Leave that for the Atroxians to do."]),
                      (L, ["I like the sound of that.", "Now, is the mission clear?"])],
    'AR_BWB_03.RAW': [(K, ["Indeed.", "I've got this:"])],
    'AR_BWB_04.RAW': [(K, ["First, I take out the Atroxian power grid, to get into the city of Kridonea...",
                           "Then I fight my way to the Great Ziggurat, and find the secret room where your people are hiding...",
                           "From there, your elder will help me get to the Sky Sanctum,"
                           " where I'll defeat Atroxus, destroy the Realm Reactor, and save the planet!"])],
    'AR_BWB_05.RAW': [(K, ["Sounds like a plan.", "Well, a really hard and long plan.", "But still, a plan."]),
                      (L, ["Good luck, Commander.", "I promise we'll fix up your ship as soon as this is over."])],
    'AR_BWB_06.RAW': [(K, ["Then it's settled.", "There's no time to waste!"]),
                      (L, ["Now, I must return to my people in hiding.", "Farewell, young one."]),
                      (K, ["See you soon!"])],
    'AR_BWB_07.RAW': [(N, ["Waving a goodbye, Keen sets off on his biggest adventure yet,"
                           " unaware of a dark intelligence monitoring him closely..."])],
    # ---- the Great Ziggurat: the elder
    'AR_ELDER_00.RAW': [(E, ["Welcome, Commander Keen.", "I am Cyraanae-Augustrus Krislen,"
                             " elder leader of the Krile, or at least those who remain."])],
    'AR_ELDER_01.RAW': [(E, ["We have suffered greatly, Commander.",
                             "When the Atroxians attacked the city, we fought valiantly, but it wasn't enough..."])],
    'AR_ELDER_02.RAW': [(E, ["Soon Kridonea fell, and those who survived took refuge here.",
                             "With all hope lost, we remained here, waiting for our end.",
                             "And then, just as we had given up, we heard that you had come to Krisle, and everything changed..."])],
    'AR_ELDER_03.RAW': [(K, ["Mister Elder, I don't even know what to say.", "Everything I've seen, it's so horrible..."]),
                        (E, ["Young one, I am forever sorry that you have to witness all this.",
                             "But yet, in our darkest hour, I see in you a beacon of hope.",
                             "You are truly the savior of our time."])],
    'AR_ELDER_04.RAW': [(K, ["I may have seen my planet, my galaxy, and even my universe threatened, but never this.",
                             "The Atroxians are going to pay for what they've done!"])],
    'AR_ELDER_05.RAW': [(E, ["Commander Keen, I'm afraid that I must ask you to face great evil.",
                             "This is no task for a child.", "Are you sure you want to do this?"])],
    'AR_ELDER_06.RAW': [(K, ["Absolutely!"]), (E, ["Very well.", "I commend your courage."])],
    'AR_ELDER_07.RAW': [(E, ["To save Krisle, we must target the Realm Reactor atop the Sky Sanctum.",
                             "It is the source of this foul darkness that has engulfed the planet.",
                             "Destroy it, and we can repel the Atroxians away from here."])],
    'AR_ELDER_08.RAW': [(E, ["However, Atroxus, the supreme Atroxian commander, will no doubt be waiting for you.",
                             "And, the Atroxians may have other surprises to use against you...",
                             "I still do not know what happened to a Krile expedition we sent to the mesa after the invasion.",
                             "This troubles me greatly..."])],
    'AR_ELDER_09.RAW': [(E, ["Now, I shall prepare you a hover transporter to take you to the top of the mesa.",
                             "Unfortunately, the transporter will not be able to pass through the epicenter"
                             " of the Atroxian Realm hovering over the sanctum, so you'll need to climb to the top yourself."])],
    'AR_ELDER_10.RAW': [(E, ["And now, the time of our reckoning has arrived.",
                             "It is time for you to head to the Sky Sanctum and finish this, if you are ready."]),
                        (K, ["Well, I am almost ten years old.", "I think I'm ready for anything!"])],
    'AR_ELDER_11.RAW': [(E, ["Then, good luck, Commander Keen.", "Our trust is eternally with you."])],
    # ---- the Sky Sanctum: Atroxus
    'AR_ATROXUS_01.RAW': [(X, ["Halt, organism!"])],
    'AR_ATROXUS_03.RAW': [(X, ["Intention to terminate Atroxian Realm suspected.",
                               "Does organism wish its demise to be devised?"])],
    'AR_ATROXUS_05.RAW': [(X, ["The Atroxians answer to no desire, only Directive X M."])],
    'AR_ATROXUS_11.RAW': [(X, ["Bioscan initiated...", "Analyzing scan data...", "Species: human.", "Planet: earth.",
                               "Subject identified as Commander Keen.", "Threat level: ultra low.",
                               "Target acquired.", "Termination function initiated."])],
    # ---- the secret crystal shrine
    'AR_SHRINE_00.RAW': [(K, ["Cyraaun Kriuus-Llquo!"])],
    'AR_SHRINE_01.RAW': [(K, ["I got that name right, didn't I?", "You guys have some really hard names to remember!"]),
                         (L, ["Indeed you did.", "Welcome."])],
    'AR_SHRINE_02.RAW': [(K, ["Good thing you're okay.", "What is this place?"]),
                         (L, ["We built this as an underground sanctuary.",
                              "When the Atroxians attacked, many of those outside the city fled here."])],
    'AR_SHRINE_03.RAW': [(K, ["You should be careful.", "The Atroxians set up some kind of mining operation down here."]),
                         (L, ["Oh no..."])],
    'AR_SHRINE_04.RAW': [(K, ["What?", "You really think they'll find you here?"]),
                         (L, ["Maybe, but I'm afraid we now have bigger things to fear."])],
    'AR_SHRINE_05.RAW': [(L, ["For centuries, Krile scientists, including myself,"
                              " have been studying the unique emeralds found in these caves.",
                              "We believe they have certain, um, quantum properties.",
                              "They contain traces of a rare element called quantium."])],
    'AR_SHRINE_06.RAW': [(K, ["Quantum?", "As in quantum entanglement?",
                              "Did you by chance detect trace amounts of ionized platnotomic plasma in the emeralds?"]),
                         (L, ["Err, actually, I believe we did.", "Any theories?"])],
    'AR_SHRINE_07.RAW': [(K, ["If I'm correct, the quantium in those emeralds is a form of exotic matter"
                              " that could be used to bend spacetime.", "We're talking about wormholes to other dimensions!",
                              "Anyone messing with the structure of the multiverse is bad news.",
                              "This could be quite the quantum quagmire!"])],
    'AR_SHRINE_08.RAW': [(L, ["You know, if anyone else but you had said that,"
                              " I would have thought they were just making up silly science..."])],
    'AR_SHRINE_09.RAW': [(L, ["Do you think the Atroxians invaded Krisle to harvest these emeralds?"]),
                         (K, ["I'm almost positive, and they must have seen your people as a threat to that mission."])],
    'AR_SHRINE_10.RAW': [(K, ["I think there may be a larger plan at work here.", "But for now, I've got to stop the Atroxians.",
                              "See you soon!"]),
                         (L, ["Farewell, Commander.", "My people are counting on you!"])],
    # ---- Atroxus at the three generators
    'AR_BOX1_00.RAW': [(X, ["Halt, organism!", "What is this intruder?", "Identify self!"])],
    'AR_BOX1_03.RAW': [(K, ["You wish.", "I'm Commander Keen, and I've come here to stop you!"])],
    'AR_BOX1_04.RAW': [(X, ["Organism matches scans of downed unidentified flying vessel.", "Organism has issued threat.",
                            "Termination protocol activated.", "Farewell, organism..."])],
    'AR_BOX1_05.RAW': [(K, ["I'm not afraid of you, Atroxus.", "That generator is toast!"])],
    'AR_BOX1_06.RAW': [(X, ["Begin security lockdown.", "Deploy guard drones.", "Protect generator core.", "Eliminate subject!"])],
    'AR_BOX1_07.RAW': [(K, ["So, you think you can stop me, Atroxus?", "We'll see about that."])],
    'AR_BOX2_00.RAW': [(X, ["Halt, organism!", "So, organism has evaded termination protocol, and aims to sabotage power grid.",
                            "It is greater nuisance than previously calculated."])],
    'AR_BOX2_02.RAW': [(X, ["It stands in opposition to Directive X M.", "It shall stand no longer."])],
    'AR_BOX2_03.RAW': [(K, ["Wait, what's Directive X M?"])],
    'AR_BOX2_04.RAW': [(X, ["Begin security lockdown.", "Deploy guard drones.", "Protect generator core.", "Eliminate subject!"])],
    'AR_BOX2_05.RAW': [(K, ["Good luck with that.", "It's time to take out the power!"])],
    'AR_BOX3_00.RAW': [(X, ["Halt, organism!",
                            "So, organism has dismantled the power grid, defeated the quarantine, and gained entrance to the city.",
                            "It believes it is closer to victory.", "It approaches only failure."])],
    'AR_BOX3_02.RAW': [(X, ["There is nothing for the organism to find in this city.",
                            "Its protectors bled in accordance with their transgressions."])],
    'AR_BOX3_03.RAW': [(K, ["No..."])],
    'AR_BOX3_04.RAW': [(X, ["Organism shall suffer the same fate.", "It shall find only death here.", "And that it shall receive."])],
    'AR_BOX3_07.RAW': [(K, ["Then, this ends!"])],
    # ---- after the fight
    'AR_END_01.RAW': [(N, ["As his engines explode, Atroxus' core bursts into a conflagration of flames and blinding light.",
                           "Keen leaps away just in time, barely avoiding the blast."])],
    'AR_END_02.RAW': [(N, ["With only a few scratches and bruises, he picks himself up,"
                           " as glistening particles sprinkle from the skies."])],
    'AR_END_03.RAW': [(N, ["Reduced to a scorched heap of metal, Atroxus' eye flickers with its last life,"
                           " and emits a garbled message:"])],
    'AR_END_04.RAW': [(X, ["Analyzing...", "Three-fourteen internal error: cannot divide by Keen.",
                           "Initiate planetary evacuation protocol.",
                           "So, pathetic little human believes itself victorious.", "It knows nothing of victory.",
                           "It is merely inconvenience to Directive X M."])],
    'AR_END_05.RAW': [(K, ["Atroxus, what is Directive X M?", "What do you want?", "Who are you working for?"])],
    'AR_END_06.RAW': [(X, ["Human will not have victory.", "Human will not learn truth of Directive X M.",
                           "Its primitive mind is of insufficient comprehension.",
                           "Its ignorance shall ensure its termination when M..."]), (None, 2.5),
                      (N, ["A fatal error has occurred.", "The current application will be terminated.",
                           "Please contact VitaCorp for assistance."])],
    'AR_END_07.RAW': [(K, ["VitaCorp?!"])],
    'AR_END_08.RAW': [(N, ["Breathing heavily, Keen stares at Atroxus' remains, trying to comprehend its last message.",
                           "What was this Directive X M, and how was VitaCorp connected to all of it?",
                           "Keen felt his head spinning with confusion.", "He'd have to figure this out later.",
                           "All that mattered now was saving Krisle..."])],
    # ---- the Realm Reactor
    'AR_END2_00.RAW': [(N, ["Keen approaches the giant Realm Reactor crystal with apprehension."]),
                       (K, ["So, how am I supposed to get rid of this thing?",
                            "My neural stunner will never be able to penetrate the exterior shield.",
                            "I need something more powerful..."])],
    'AR_END2_01.RAW': [(N, ["Returning to the remains of Atroxus, Keen sorts through the charred rubble,"
                            " and pulls out a broken crystal-weapon energizer."]),
                       (K, ["Perfect!", "This little guy's still intact.",
                            "Just have to pull apart a few wires, fuse this connection, recalibrate the exhibitor sensor, and...",
                            "Presto.", "My own proto-plasmid blaster!"])],
    'AR_END2_02.RAW': [(K, ["If you could have felt anything at all, Atroxus, I bet you'd have found this ironic!"])],
    'AR_END2_06.RAW': [(N, ["As the Atroxian Realm vaporizes, the Sky Sanctum shines with a golden hue of light."])],
    'AR_END2_07.RAW': [(N, ["Keen stares at the skies, watching the last remnants of the realm vortexes dissipate.",
                            "It was finally over.",
                            "After collecting some hardware from Atroxus for later study,"
                            " he summons the transport back to the Great Ziggurat..."])],
    # ---- the ending in the Great Ziggurat
    'AR_NORMALEND_00.RAW': [(N, ["As Keen proceeds into the main hall of the Ziggurat, a procession of Krile march forward, bowing to Keen.",
                                 "From the front of the line, Elder Cyraanae-Augustrus Krislen steps forward."])],
    'AR_NORMALEND_01.RAW': [(E, ["Commander, let this honor guard be a tribute to the bravery and heroism you have shown today.",
                                 "Thanks to you, our exile has ended.", "We are eternally grateful."]),
                            (K, ["Thank you, Mister Elder.", "I guess they're all gone then?"])],
    'AR_NORMALEND_02.RAW': [(E, ["It appears so.", "As the realm dissipated, the Atroxians jettisoned escape pods,"
                                 " and retreated through a wormhole.",
                                 "However, many of them never made it far off the ground, thanks to a power failure."]),
                            (K, ["Well, they have me to thank for that."])],
    'AR_NORMALEND_03.RAW': [(E, ["Commander, one thing still troubles me.", "Why would the Atroxians want to attack us?",
                                 "Why go to so much effort to invade this planet?"])],
    'AR_NORMALEND_04.RAW': [(K, ["I've been trying to figure that out myself.", "Atroxus mentioned something about a Directive X M.",
                                 "And, when I busted him up, his software program mentioned none other than VitaCorp."])],
    'AR_NORMALEND_05.RAW': [(E, ["I'm afraid I know nothing about Directive X M.",
                                 "But, I have heard of this VitaCorp, and they sound like trouble."]),
                            (K, ["I should know.", "They supplied the Shikadi with an armageddon machine to wipe out my entire galaxy.",
                                 "Or at least they tried.", "Pretty shoddy craftsmanship, if you ask me."])],
    'AR_NORMALEND_06.RAW': [(E, ["If VitaCorp is behind this, what did they want with us?"])],
    'AR_NORMALEND_07.RAW': [(K, ["I think they just wanted you out of the way, to exploit this planet.",
                                 "The Atroxians built an intricate network of transportation, connected to an underground mining facility."]),
                            (E, ["Of course...", "No doubt they were harvesting the rare emeralds found on this planet.",
                                 "They contain a rare and dangerous element called quantium."])],
    'AR_NORMALEND_08.RAW': [(K, ["Uh oh.", "VitaCorp must have a large supply of this quantium by now.", "I've got to stop them!"]),
                            (E, ["Indeed.", "We may have won, but the masterminds of this operation are still out there.",
                                 "No one is safe until they've been brought to justice."])],
    'AR_NORMALEND_09.RAW': [(K, ["I agree.", "Just one problem: VitaCorp is astronomically huge.",
                                 "Whatever division is responsible will no doubt go to great lengths to hide the truth.",
                                 "This could take a very long time to crack."]),
                            (E, ["I have every faith in you.", "But now, I must attend to my people.",
                                 "I shall leave you with my trusted advisor.", "Farewell, Commander."])],
    'AR_NORMALEND_10.RAW': [(L, ["Hello again, young one!"]), (K, ["Cyraaun Kriuus-Llquo!"]),
                            (L, ["I'm so happy you made it out okay.", "Come, follow me.", "I have something to show you."])],
    'AR_NORMALEND_12.RAW': [(K, ["Woah, that's incredible!", "I don't know how you did it, but you guys are amazing!"]),
                            (L, ["I'm glad you like it.", "It's the least we could do.", "There are even a few modifications!"]),
                            (K, ["I can't believe you managed to rebuild it so quickly!"])],
    'AR_NORMALEND_13.RAW': [(L, ["Err, yes...", "rebuild.", "Look, ah, we just decided to order a new one."]),
                            (K, ["What!", "You can't just order a new Bean-with-Bacon Megarocket.", "I invented it!"])],
    'AR_NORMALEND_14.RAW': [(L, ["Well, uh, Sklargifax Space Industries just released a new line of spaceship models, based on yours.",
                                 "I guess you've become quite popular."]),
                            (K, ["But...", "the B W B is copyrighted!", "They can't just mod my design and sell it!",
                                 "That's infringement!"])],
    'AR_NORMALEND_15.RAW': [(L, ["You should probably do something about that.", "But, at least it got you a ship."]),
                            (K, ["It may be a cheap knock-off, but I do appreciate the thought.",
                                 "I'm going to be having a word with these Sklargifolks..."])],
    'AR_NORMALEND_16.RAW': [(K, ["Well, thank you all for everything.", "But, honestly, I'm so ready to get off this planet."]),
                            (L, ["Then my people and I shall bid you farewell, Commander Keen.",
                                 "I would say thank you again, but I kind of feel we've worn that into the ground."]),
                            (K, ["No problemo, mister.", "I'll keep in touch.", "See ya!"])],
    'AR_NORMALEND_17.RAW': [(N, ["Waving one last goodbye, Keen fires up his brand new Bean-with-Bacon megarocket.",
                                 "As he blasts off into space, the Krile give one final salute."])],
    'AR_NORMALEND_19.RAW': [(N, ["Exhausted, Keen slumps in the cockpit of the new B W B, and charts a course for Earth.",
                                 "New mysteries lay ahead, but for now he needed a good rest.",
                                 "He resolved to investigate VitaCorp's dark secret.",
                                 "But, saving an entire race of aliens was enough for one day."])],
}


def main(folder):
    os.makedirs(folder, exist_ok=True)
    raw = ['-f', 's16le', '-ar', str(RATE), '-ac', '1']
    with tempfile.TemporaryDirectory() as tmp:
        by_voice, parts = {}, {}
        for name, turns in CLIPS.items():
            for i, (voice, words) in enumerate(turns):
                if voice is None:
                    parts.setdefault(name, []).append((None, words))   # a pause of so many seconds
                    continue
                seg = 'SEG_%s_%d.RAW' % (name[:-4], i)
                by_voice.setdefault(voice, {})[seg] = words
                parts.setdefault(name, []).append((voice, seg))
        for voice, segs in by_voice.items():
            speak_all(segs, voice, tmp, **STYLE.get(voice, {}))
            if voice in EFFECTS:
                for seg in segs:
                    path = os.path.join(tmp, seg)
                    subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', path, '-af', EFFECTS[voice]] + raw + [path + '.fx'], check=True)
                    os.replace(path + '.fx', path)
        for name, segs in parts.items():
            out = bytearray()
            for k, (voice, seg) in enumerate(segs):
                if voice is None:
                    out += bytes(2 * int(seg * RATE))
                    continue
                if k:
                    out += bytes(2 * int(GAP * RATE))
                out += open(os.path.join(tmp, seg), 'rb').read()
            open(os.path.join(folder, name), 'wb').write(out)
            print('%-26s %5.1f s' % (name, len(out) / 2 / RATE))
    mine = [f for f in os.listdir(folder) if f.startswith('AR_')]
    print('%d clips, %.1f MB' % (len(mine), sum(os.path.getsize(os.path.join(folder, f)) for f in mine) / 1e6))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
