# frontend-gs (Gymspace web)

> ⚠️ **Toto je ukázková/demo verze** – projekt obsahuje pouze **část komponent** a funkcionalit.

Frontend aplikace pro **Gymspace** – webová aplikace postavená na **Vue 3 + TypeScript** s **Vite**.

---

## Tech stack

- **Vue 3** + **TypeScript**
- **Vite** (build & dev server)

---

## Požadavky

- **Node.js** (doporučeně LTS) + **npm**

---

## Rychlý start

> ⚠️ **Aplikace se nedá spustit** – jedná se pouze o **kód snippety** a příklady komponenty. 
> Slouží k demonstraci struktury projektu, architektur a stylu kódování.

Pro referenci by se projekt spouštěl takto:

```bash
npm install
npm run dev
```

## Skripty & běžné příkazy

### Web (build)
```bash
npm run build
# nebo
vite build
```

## Struktura repozitáře

- `src/` – hlavní Vue aplikace
  - `view/` – Vue komponenty (stránky/sekce)
  - `logic/` – business logic, API, utilities
  - `assets/` – obrázky, ikony, fonty
  - `theme/` – CSS a styling
  - `router/` – Vue Router konfigurace
- `index.html` – vstupní HTML
- `package.json` – závislosti a skripty

---

## Commit pravidla (rychlá verze)

Používej prefixy pro rychlou orientaci v historii:

- `feat:` nová funkcionalita  
- `fix:` oprava chyby  
- `chore:` údržba / závislosti  
- `refactor:` úprava struktury bez změny chování  
- `docs:` dokumentace  
- `test:` testy  
- `style:` formátování  
- `perf:` výkon  
- `ci:` CI/CD  
- `build:` build tooling  
- `revert:` revert  
- `hotfix:` rychlá oprava produkce  

Příklad:
```text
feat: Přidává registraci přes OAuth
```
