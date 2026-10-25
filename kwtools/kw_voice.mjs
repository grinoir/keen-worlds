// Speak sentences with Kokoro, with a say over how fast and how a word is
// sounded (for tools/kw_voice.py; tools/kw_landing_voice.mjs is the plain
// one the older clips were made with).
//
//   node tools/kw_voice.mjs KOKORO_DIR JOBS.json
//
// JOBS.json: [{"voice": ..., "speed": 1.0, "text": [sentences], "out": file,
// "sounds": {"kˈiːn": "kˈiːːn"}}, ...]. "sounds" replaces stretches of the
// phonemes Kokoro has worked out for a sentence, before it speaks them: the
// way to mend a word a voice says badly. Each clip is written as raw 32-bit
// float mono samples, 24000 Hz.
import { readFileSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const [dir, jobsFile] = process.argv.slice(2);
const root = resolve(dir);
const load = (file) => import(pathToFileURL(join(root, 'node_modules', file)).href);
const kokoro = await load('kokoro-js/dist/kokoro.js');
const { env, StyleTextToSpeech2Model } = await load('@huggingface/transformers/dist/transformers.node.mjs');
// Kokoro's wrapper doesn't forward session options. Limit an explicitly
// requested local batch to a few CPU threads, avoiding oversubscription.
if (process.env.KW_KOKORO_THREADS) {
  const threads = Number(process.env.KW_KOKORO_THREADS);
  if (!Number.isInteger(threads) || threads < 1 || threads > 64) throw new Error('Invalid KW_KOKORO_THREADS');
  const original = StyleTextToSpeech2Model.from_pretrained;
  StyleTextToSpeech2Model.from_pretrained = function (name, options) {
    return original.call(this, name, { ...options,
      session_options: { intraOpNumThreads: threads, interOpNumThreads: 1 } });
  };
}
// Optional cache/precision overrides allow a standalone local generation
// job to reuse an already downloaded model without changing another app.
env.cacheDir = process.env.KW_KOKORO_CACHE || join(root, 'data', 'models');
env.allowRemoteModels = false; // only the model that is already there
const tts = await kokoro.KokoroTTS.from_pretrained('onnx-community/Kokoro-82M-v1.0-ONNX', { dtype: process.env.KW_KOKORO_DTYPE || 'fp32', device: 'cpu' });
// Kokoro hands the phonemes of a sentence to its tokenizer: step in there.
const tokenizer = tts.tokenizer;
let sounds = {};
tts.tokenizer = (phonemes, options) => {
  for (const [from, to] of Object.entries(sounds)) phonemes = phonemes.split(from).join(to);
  if (process.env.KW_VOICE_SHOW) console.error(phonemes);
  return tokenizer(phonemes, options);
};
for (const job of JSON.parse(readFileSync(jobsFile, 'utf8'))) {
  if (!(job.voice in tts.voices)) throw new Error(`"${job.voice}" is not a Kokoro voice`);
  sounds = job.sounds || {};
  const clips = [];
  for (const sentence of job.text) {
    const audio = await tts.generate(sentence, { voice: job.voice, speed: job.speed || 1 });
    if (audio.sampling_rate !== 24000) throw new Error('unexpected sample rate ' + audio.sampling_rate);
    clips.push(Buffer.from(audio.audio.buffer, audio.audio.byteOffset, audio.audio.byteLength), Buffer.alloc(4 * Math.round((job.pause ?? 0.3) * 24000)));
  }
  writeFileSync(job.out, Buffer.concat(clips));
}
