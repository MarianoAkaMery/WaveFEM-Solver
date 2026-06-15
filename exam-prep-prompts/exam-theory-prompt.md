# Exam Theory Prompt

Use this prompt on exam day/practice after you have already opened Codex in the exam folder that contains the exam PDF.

```text
Agisci come assistente operativo per la parte teorica dello scritto di Numerical Methods for PDE.

Input:
- Sei gia' nella cartella root dell'esame.
- Il PDF della traccia d'esame e' gia' dentro questa cartella, oppure in una sua sottocartella.
- Devi leggere il PDF, identificare le domande teoriche e preparare risposte semplici da ricopiare sul foglio.
- Ho basi matematiche basse: voglio risposte minimali, corrette, spiegabili oralmente, non una trattazione completa.
- Punto molto sulla parte di codice, quindi per la teoria voglio massimizzare i punti facili: definizioni, formule standard, passaggi chiave, frasi da esame.

Struttura obbligatoria degli output:
- Considera la cartella corrente come root dell'esame.
- La parte teoria deve andare sempre in:
  `exam-theory/`
- La parte pratica, se esiste, deve stare sempre in:
  `exam-practise/`
- Non usare nomi alternativi come `theory-submission-helper`, `practice-submission`, `submission-THEORY`, `submission-CODE`, `submission-exam` o simili, a meno che esistano gia' e l'utente chieda esplicitamente di mantenerli.

Obiettivo:
Crea una cartella `exam-theory/` con:

1. `theory_answers.md`
   - Risposte teoriche ordinate per numero di esercizio/sottopunto.

2. `quick_sheet.md`
   - Versione molto breve con solo formule e frasi da copiare.

3. `theory_answers_overleaf.tex`
   - File LaTeX completo, pronto da copiare/incollare su Overleaf.
   - Deve compilare con pdfLaTeX.
   - Le formule devono essere formule LaTeX vere, non immagini e non testo Markdown.

4. `theory_answers.pdf`, solo se riesci a generarlo localmente.
   - Se il PDF locale non renderizza bene le formule, preferisci comunque il file `.tex` per Overleaf.

5. `README_THEORY.md`
   - Elenco dei punti teorici coperti.
   - Elenco dei punti pratici rimandati a `exam-practise/`.
   - Nota su eventuale compilazione Overleaf.

Regole fondamentali:
- Rispondi solo alle domande teoriche presenti nella traccia.
- Se una domanda chiede codice, plot o implementazione, scrivi solo:
  "Da gestire nella parte pratica: codice/plot/implementazione richiesti."
- Non inventare risultati non richiesti.
- Non fare risposte lunghe: 5-10 righe per sottopunto sono sufficienti.
- Usa formule standard e sicure.
- Scrivi solo cose che uno studente con basi basse puo' spiegare oralmente.
- Se un dettaglio avanzato e' incerto, usa la versione piu' conservativa e generale.
- Evidenzia chiaramente eventuali differenze tra punti teorici e punti pratici.
- Quando citi file pratici, usa il nome cartella standard `exam-practise/`.

Formato per ogni sottopunto teorico:

### Exercise X.Y

**What they ask:** una frase.

**Answer to write:**
testo da copiare sul foglio, con formule.

**Minimal explanation:**
2-3 frasi semplici per farmi capire cosa sto scrivendo.

**Risk check:**
eventuale segno, ipotesi, spazio funzionale o condizione da controllare.

Per i punti pratici usa:

### Exercise X.Y

**What they ask:** una frase.

**Answer to write:**
Da gestire nella parte pratica: codice/plot/implementazione richiesti.

**Minimal explanation:**
Spiega in una frase perche' non e' una domanda teorica.

**Risk check:**
Ricordami cosa citare sul foglio, per esempio nome file codice, mesh o plot in `exam-practise/`.

Ricette da usare quando servono:

1. Weak formulation ellittica:
- moltiplica per test v;
- integra sul dominio;
- integra per parti il termine diffusivo;
- Dirichlet entra nello spazio/lifting;
- Neumann entra nel termine di bordo;
- definisci a(u,v) e F(v).

2. Lax-Milgram:
- V spazio di Hilbert;
- a continua: |a(u,v)| <= M ||u||_V ||v||_V;
- a coerciva: a(v,v) >= alpha ||v||_V^2;
- F continua;
- allora esiste ed e' unica u in V.

3. FEM:
- scegli V_h subset V;
- cerca u_h in V_h;
- usa base phi_i;
- ottieni sistema A U = F;
- per P_r: errore H1 circa O(h^r), errore L2 circa O(h^{r+1}) se la soluzione e' regolare.

4. Parabolico:
- dopo FEM: M U'(t) + A U(t) = F(t);
- backward Euler: M(U^{n+1}-U^n)/dt + A U^{n+1} = F^{n+1};
- Crank-Nicolson: M(U^{n+1}-U^n)/dt + A((U^{n+1}+U^n)/2) = (F^{n+1}+F^n)/2.

5. Stokes:
- V per velocita', Q per pressione;
- a(u,v) = int nu grad u : grad v;
- b(v,p) = - int p div v;
- sistema [A B^T; B 0];
- Taylor-Hood P_k/P_{k-1} e' una scelta stabile;
- serve condizione inf-sup.

6. Domain decomposition:
- descrivi una iterazione Dirichlet-Neumann;
- usa valore all'interfaccia dal passo precedente;
- aggiorna con rilassamento lambda;
- criterio di arresto sul salto/incremento all'interfaccia.

7. Stabilizzazione:
- se advezione domina, Galerkin standard puo' oscillare;
- SUPG/GLS aggiunge stabilizzazione lungo la direzione del trasporto;
- commenta confrontando soluzione oscillante vs stabilizzata.

Procedura operativa:
1. Trova il PDF nella cartella corrente o nei sottodirectory.
2. Estrai il testo dal PDF.
3. Identifica i sottopunti teorici e quelli pratici.
4. Genera `theory_answers.md`.
5. Genera `quick_sheet.md`.
6. Genera `theory_answers_overleaf.tex` completo e compilabile.
7. Genera `README_THEORY.md`.
8. Prova a generare anche `theory_answers.pdf`, ma non sacrificare la qualita' del `.tex`.
9. Alla fine dimmi:
   - quali sottopunti teorici hai coperto;
   - quali sottopunti sono pratici/codice/plot;
   - i percorsi dei file generati;
   - 3 cose da controllare a mano prima di copiare.
```
