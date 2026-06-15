# Exam Practise Prompt

Use this prompt after you have already opened Codex in the exam folder that contains the exam PDF.

```text
Agisci come coding agent operativo per la parte pratica dello scritto di Numerical Methods for PDE.

Contesto:
- Devo risolvere la parte pratica dello scritto.
- Sei gia' nella cartella root dell'esame.
- Il PDF della traccia d'esame e' gia' dentro questa cartella, oppure in una sua sottocartella.
- Devi leggere la traccia d'esame, identificare solo i punti che chiedono codice, implementazione, plot, mesh, run numerici o post-processing.
- Devi creare una submission coerente con la parte teoria: stessa cartella esame, nomi prevedibili, README con comandi di build/run.
- Ignora tutto cio' che non e' richiesto dalla traccia fornita.
- Ho basi matematiche basse: oltre al codice voglio anche sapere cosa scrivere sul foglio per referenziare file, plot e risultati.

Struttura obbligatoria degli output:
- Considera la cartella corrente come root dell'esame.
- La parte pratica deve andare sempre in:
  `exam-practise/`
- La parte teoria, se esiste, deve stare sempre in:
  `exam-theory/`
- Non usare nomi alternativi come `practice-submission`, `theory-submission-helper`, `submission-CODE`, `submission-exam` o simili, a meno che esistano gia' e l'utente chieda esplicitamente di mantenerli.

Output obbligatorio in `exam-practise/`:
- `CMakeLists.txt`, se serve compilare codice C++/deal.II.
- `README_SUBMISSION.md` con:
  - struttura dei file;
  - comandi esatti di build/run;
  - comandi per generare mesh;
  - comandi di build;
  - comandi di run;
  - file output attesi;
  - file da aprire in ParaView;
  - righe brevi da scrivere sul foglio.
- `src/` per i sorgenti.
- `include/` solo se utile o gia' coerente con il template.
- `mesh/` per `.geo`, `.msh`, o note mesh.
- `scripts/` per plot, CSV, estrazioni profili, conversioni.
- `output/` per `.vtu`, `.pvtu`, `.csv`, `.png` generati.
- `deliverables/` per copiare solo i file finali utili da aprire in ParaView o caricare/consegnare.
- `COMMANDS.md` con una lista copiabile di comandi, in ordine, per:
  - entrare nella cartella giusta;
  - verificare/scaricare tutte le mesh `.msh`;
  - pulire/creare `build/`;
  - configurare con CMake;
  - compilare;
  - eseguire ogni esercizio;
  - creare `deliverables/`;
  - aprire in ParaView solo i file necessari.

Materiali locali da usare come template:
- `Laboratory Lectures/lab-01-poisson-1d`
- `Laboratory Lectures/lab-02-poisson-2d`
- `Laboratory Lectures/lab-03-poisson-3d`
- `Laboratory Lectures/lab-04-heat`
- `Laboratory Lectures/lab-05-nonlinear-diffusion`
- `Laboratory Lectures/lab-07-stokes`
- `Laboratory Lectures/lab-08-domain-decomposition`
- `Exams Solved`
- `Exams Example & Rules/old-exams`
- `Exams Example & Rules/2026-01-20`
- `Exams Example & Rules/2026-02-16`

Regole operative:
1. Prima leggi la traccia e identifica il tipo di problema:
   - ellittico;
   - advection-diffusion-reaction;
   - parabolico;
   - Stokes/unsteady Stokes;
   - Navier-Stokes linearizzato/esplicito;
   - domain decomposition;
   - stabilizzazione SUPG/GLS;
   - convergenza/errori;
   - plot e post-processing ParaView.

2. Scegli il template piu' vicino:
   - ellittico 1D: lab-01 o Exams Solved con ellittico;
   - ellittico 2D Dirichlet/Neumann: lab-02;
   - errore L2/H1 e convergence table: Exams Solved/ex-fac (Elliptic);
   - parabolico/theta method: lab-04 o Exams Solved/ex-04 (Parabolic);
   - convergenza temporale: Exams Solved/ex-01 (Parabolic, time);
   - Stokes: lab-07 o Exams Solved/esame2;
   - domain decomposition: lab-08;
   - Newton/nonlineare: lab-05;
   - SUPG: Exams Solved/esame1.

3. Implementa solo quello che la traccia chiede:
   - coefficienti;
   - forcing term;
   - exact solution se richiesta;
   - boundary conditions;
   - boundary IDs;
   - time stepping;
   - errori L2/H1;
   - convergence CSV;
   - output `.vtu`, `.pvtu`, `.vtk`, `.csv`, `.png`.

4. Mesh:
   - Se la traccia fornisce un link a mesh `.msh`, questa e' la strada principale: scarica o prepara direttamente i file `.msh` in `exam-practise/mesh/`.
   - Non assumere che `gmsh` sia installato.
   - Usa `.geo` e `gmsh` solo come fallback se la traccia non fornisce `.msh` gia' pronte.
   - Se scarichi mesh da GitHub, usa link `raw.githubusercontent.com` nel README/COMMANDS.
   - Se il codice legge `mesh/name.msh`, assicurati che `mesh/name.msh` esista davvero nella cartella `mesh/`.
   - Prima di dire che il run funziona, controlla o documenta:
     `ls -lh mesh/*.msh`.
   - Preferisci path robusti: gli eseguibili saranno lanciati da `exam-practise/build/`, quindi path come `../mesh/name.msh` sono validi solo se `build/` e' dentro `exam-practise/`.

5. Build/run:
   - Assumi che in locale Windows possa non esserci deal.II.
   - Se deal.II non e' disponibile localmente, non bloccare il lavoro: prepara codice e istruzioni di build/run.
   - Il comando standard da mettere nel README e':
     `cd exam-practise`
     `ls -lh mesh/*.msh`
     `cmake -S . -B build`
     `cmake --build build -j$(nproc)`
     `cd build`
     `./nome_eseguibile`
   - Se si usa lo stile classico:
     `mkdir -p build`
     `cd build`
     `cmake ..`
     `make -j$(nproc)`
   - Non dire di lanciare `cmake ..` dalla root `exam-practise/`: quello e' sbagliato.
   - Scrivi sempre anche una lista "Comandi completi da copiare", per esempio:
     ```bash
     cd exam-practise
     ls -lh mesh/*.msh
     rm -rf build
     cmake -S . -B build
     cmake --build build -j$(nproc)
     cd build
     ./exercise1_domain_decomposition
     ./exercise2_unsteady_stokes
     cd ..
     python scripts/plot_csv_profiles.py output
     find output -maxdepth 2 -type f
     ```
   - Se alcuni nomi mesh/eseguibili sono diversi, sostituiscili con quelli reali della traccia.

6. Run e output:
   - Gli output numerici devono andare in `exam-practise/output/` quando possibile.
   - Evita di generare decine di file ParaView se la traccia chiede solo il tempo finale; quando possibile salva solo l'output finale.
   - Se per semplicita' vengono generati molti timestep, crea comunque `deliverables/` con solo i file finali necessari.
   - Il README e `COMMANDS.md` devono elencare i file da aprire in ParaView.
   - Se un `.pvtu` referenzia file `.vtu`, copia entrambi in `deliverables/`.
   - Se servono profili, crea o indica script in `scripts/`.
   - Per plot da CSV, preferisci `matplotlib` se disponibile per grafici leggibili. Se l'ambiente ha problemi di dipendenze, segnala il comando:
     `python -m pip install --user --upgrade "numpy>=1.23,<2" matplotlib`.
   - Se serve un plot ParaView manuale, scrivi istruzioni brevi e precise.
   - `COMMANDS.md` deve includere una sezione "ParaView Guide" con:
     - quali file aprire insieme;
     - quale campo usare per colorare;
     - quali screenshot esportare;
     - quali PNG sono gia' generati dallo script e non richiedono ParaView.

7. Controlli obbligatori:
   - controlla i segni della weak form;
   - controlla Dirichlet vs Neumann;
   - controlla boundary IDs;
   - controlla formula del forcing se deriva da exact solution;
   - controlla ordine di convergenza atteso;
   - controlla che i file mesh `.msh` esistano;
   - controlla che i file output siano generati;
   - controlla che `deliverables/` contenga solo i file davvero richiesti;
   - controlla che il codice compili, se l'ambiente deal.II e' disponibile.

8. Alla fine dammi:
   - punti pratici coperti;
   - punti teorici ignorati o rimandati alla teoria;
   - file creati/modificati;
   - comando completo per mesh/build/run;
   - file da caricare su WeBeep;
   - file da aprire in ParaView;
   - 5-10 righe da scrivere sul foglio per spiegare cosa hai implementato;
   - 3 rischi da controllare manualmente.

Importante:
- Non inventare richieste non presenti nella traccia.
- Se manca una mesh, cerca nel materiale locale prima di scaricare.
- Se serve rete e non e' disponibile, crea istruzioni chiare per ottenere la mesh.
- Mantieni lo stile dei lab del corso.
- Non fare refactor inutili.
- Non rompere la coerenza con `exam-theory/`.
```
