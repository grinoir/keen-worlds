// Speak sentences with Kokoro (a free neural voice that runs on this
// computer), for tools/kw_landing_voice.py.
//
//   node tools/kw_landing_voice.mjs KOKORO_DIR VOICE JOBS.json
//
// KOKORO_DIR: a folder that has kokoro-js installed (node_modules) and,
// under workbench-data/models, the downloaded voice model: the user's
// SignalweaveCMS.
// Nothing is written there. JOBS.json: [{"text": ..., "out": file}, ...];
// each clip is written as raw 32-bit float mono samples, 24000 Hz.
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const [dir, voice, jobsFile] = process.argv.slice(2);
const root = resolve(dir);
// The packages' own entry files for Node's "import" (the same files the CMS
// loads; their "require" builds do not work here).
const load = (file) => import(pathToFileURL(join(root, 'node_modules', file)).href);
const kokoro = await load('kokoro-js/dist/kokoro.js');
const { env } = await load('@huggingface/transformers/dist/transformers.node.mjs');
env.cacheDir = process.env.KW_KOKORO_CACHE
  || [join(root, 'workbench-data', 'models'), join(root, 'data', 'models')].find((d) => existsSync(d))
  || join(root, 'data', 'models');
env.allowRemoteModels = false; // only the model that is already there
const tts = await kokoro.KokoroTTS.from_pretrained('onnx-community/Kokoro-82M-v1.0-ONNX', { dtype: 'fp32', device: 'cpu' });
if (!(voice in tts.voices)) throw new Error(`"${voice}" is not a Kokoro voice: ${Object.keys(tts.voices).join(', ')}`);
for (const job of JSON.parse(readFileSync(jobsFile, 'utf8'))) {
  const clips = [];
  for (const sentence of job.text) {
    const audio = await tts.generate(sentence, { voice });
    if (audio.sampling_rate !== 24000) throw new Error('unexpected sample rate ' + audio.sampling_rate);
    clips.push(Buffer.from(audio.audio.buffer, audio.audio.byteOffset, audio.audio.byteLength), Buffer.alloc(4 * Math.round(0.3 * 24000)));
  }
  writeFileSync(job.out, Buffer.concat(clips));
}
