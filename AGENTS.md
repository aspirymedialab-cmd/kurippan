# Working notes for agents in this repository

## What this app is
A Vite + React 19 + TypeScript single-page app ("Tamil Word Editor", exported from
Google AI Studio). `npm run dev` (`vite --port=3000 --host=0.0.0.0`) is the entry
point; there is no backend process — `@google/genai` is called from the browser.

## Sandbox / dev environment (Base44)
- Run it with `docker compose -f docker-compose.base44.yml up -d` (host port 3000).
  A `node:22` container bind-mounts the repo at `/app`, keeps `node_modules` in a
  named volume, and runs `npm ci && npm run dev` on every start.
- Secrets reach the container as **environment variables** via
  `/run/base44/app.env` (outside the repo). Vite's `loadEnv` only reads `.env*`
  files, so `vite.config.ts` falls back to `process.env.GEMINI_API_KEY`; without
  that fallback a dashboard-provided key never reaches the client bundle.
- `GEMINI_API_KEY` is required for the app's AI features but not to boot. The
  README's `.env.local` step is the AI Studio way; under Base44 use the secrets
  dashboard instead and leave the key out of compose `environment:`.
- The preview proxy serves host port 3000; Vite's allowed hosts come from the
  platform-provided `__VITE_ADDITIONAL_SERVER_ALLOWED_HOSTS`.
- Verify a boot with `docker compose -f docker-compose.base44.yml ps` plus
  `curl -sI http://localhost:3000/`.

## Known issue: missing application source (as of this setup)
`index.html` loads `/src/main.tsx`, but **no `src/` directory exists in any branch
of this repository** — only `index.html`, `package.json`, `package-lock.json`,
`vite.config.ts`, `tsconfig.json`, `metadata.json`, `README.md` and `LICENSE` were
uploaded. Consequences:
- The dev server starts and serves `index.html`, but the module request returns
  404, so the page renders blank with a console error.
- `curl -s -o /dev/null -w '%{http_code}' http://localhost:3000/src/main.tsx`
  returns 404 until the source is added. Re-check this first when the preview is
  blank.
- Do not scaffold a replacement UI: the real source lives in AI Studio
  (app id `ba82cc64-c7c2-4f5d-97d2-da881a6a3ba0`, see README). Once it is added,
  no compose change is needed.
