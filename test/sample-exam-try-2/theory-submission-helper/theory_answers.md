# Theory answers - sample exam try 2

Source: `exam-prep-prompts/sample-exam-try-2/Exam.pdf`

Punti teorici coperti: 1.1, 1.2, 1.3, 1.4, 2.1, 2.2, 2.3, 2.4.  
Punti pratici/codice/plot: 1.5, 1.6, 2.5, 2.6.

## Exercise 1

### Exercise 1.1

**What they ask:** Scrivere la formulazione debole del problema ellittico.

**Answer to write:**
Assumo \(f\in L^2(\Omega)\) e pongo
\[
V=H^1_0(\Omega).
\]
Cercare \(u\in V\) tale che, per ogni \(v\in V\),
\[
a(u,v)=F(v),
\]
dove
\[
a(u,v)=\alpha\int_\Omega \nabla u\cdot\nabla v\,dx
+\gamma\int_\Omega uv\,dx,
\qquad
F(v)=\int_\Omega fv\,dx.
\]

**Minimal explanation:**
Moltiplico la PDE per una funzione test \(v\), integro su \(\Omega\), e integro per parti il termine diffusivo. Il termine di bordo sparisce perche' \(v=0\) su \(\partial\Omega\). La condizione di Dirichlet omogenea entra nello spazio \(H^1_0(\Omega)\).

**Risk check:**
Serve assumere \(\Omega\) abbastanza regolare, per esempio limitato e Lipschitz. Se \(f\) non e' in \(L^2\), basta scrivere piu' in generale \(F\in V'\).

### Exercise 1.2

**What they ask:** Provare esistenza e unicita' della soluzione debole.

**Answer to write:**
Uso Lax-Milgram su \(V=H^1_0(\Omega)\). La forma bilineare e' continua:
\[
|a(u,v)|\le C\|u\|_{H^1(\Omega)}\|v\|_{H^1(\Omega)}.
\]
Inoltre e' coerciva, perche' usando Poincare:
\[
a(v,v)=\alpha\|\nabla v\|^2_{L^2(\Omega)}
+\gamma\|v\|^2_{L^2(\Omega)}
\ge \alpha\|\nabla v\|^2_{L^2(\Omega)}
\ge c\|v\|^2_{H^1(\Omega)}.
\]
Il funzionale \(F\) e' continuo:
\[
|F(v)|\le \|f\|_{L^2(\Omega)}\|v\|_{L^2(\Omega)}
\le C\|f\|_{L^2(\Omega)}\|v\|_{H^1(\Omega)}.
\]
Quindi esiste ed e' unica \(u\in V\).

**Minimal explanation:**
Lax-Milgram richiede continuita', coercivita' e termine noto continuo. La coercivita' non richiede \(\gamma>0\), perche' \(\alpha>0\) e la disuguaglianza di Poincare bastano in \(H^1_0(\Omega)\).

**Risk check:**
Non scrivere che serve \(\gamma>0\): il testo dice solo \(\gamma\ge 0\). Il motivo sicuro e' \(\alpha>0\) piu' Poincare.

### Exercise 1.3

**What they ask:** Approssimare con FEM di grado 2 e dare stabilita' ed errore.

**Answer to write:**
Sia \(\mathcal T_h\) una mesh di \(\Omega\). Scelgo lo spazio \(P_2\)
\[
V_h=\{v_h\in H^1_0(\Omega): v_h|_K\in\mathbb P_2(K)\ \forall K\in\mathcal T_h\}.
\]
Cercare \(u_h\in V_h\) tale che
\[
a(u_h,v_h)=F(v_h)\qquad \forall v_h\in V_h.
\]
Con una base \(\{\varphi_i\}\) si ottiene \(AU=F\), con
\[
A_{ij}=\alpha\int_\Omega \nabla\varphi_j\cdot\nabla\varphi_i\,dx
+\gamma\int_\Omega \varphi_j\varphi_i\,dx,
\qquad
F_i=\int_\Omega f\varphi_i\,dx.
\]
Stabilita':
\[
\|u_h\|_{H^1(\Omega)}\le C\|f\|_{L^2(\Omega)}.
\]
Se \(u\in H^3(\Omega)\), per elementi \(P_2\):
\[
\|u-u_h\|_{H^1(\Omega)}\le Ch^2|u|_{H^3(\Omega)},\qquad
\|u-u_h\|_{L^2(\Omega)}\le Ch^3|u|_{H^3(\Omega)}.
\]

**Minimal explanation:**
Il metodo FEM usa la stessa formulazione debole, ma ristretta a uno spazio finito \(V_h\). La stabilita' viene dalla coercivita'. Gli ordini \(h^2\) in \(H^1\) e \(h^3\) in \(L^2\) sono quelli standard per elementi quadratici se la soluzione e' regolare.

**Risk check:**
La stima \(L^2\) richiede regolarita' sufficiente della soluzione e del problema duale. Se la soluzione e' meno regolare, gli ordini possono peggiorare.

### Exercise 1.4

**What they ask:** Scrivere una iterazione Dirichlet-Neumann su due sottodomini e dire se converge.

**Answer to write:**
Sia \(\Gamma=\partial\Omega_1\cap\partial\Omega_2\) l'interfaccia. Dato un valore iniziale \(g^k\) su \(\Gamma\):

1. Risolvo in \(\Omega_1\)
\[
-\operatorname{div}(\alpha\nabla u_1^{k+1})+\gamma u_1^{k+1}=f,
\]
con \(u_1^{k+1}=0\) sul bordo esterno e \(u_1^{k+1}=g^k\) su \(\Gamma\).

2. Risolvo in \(\Omega_2\)
\[
-\operatorname{div}(\alpha\nabla u_2^{k+1})+\gamma u_2^{k+1}=f,
\]
con \(u_2^{k+1}=0\) sul bordo esterno e condizione di flusso
\[
\alpha\partial_{n_2}u_2^{k+1}
=-\alpha\partial_{n_1}u_1^{k+1}\quad\text{su }\Gamma.
\]

3. Aggiorno il dato di interfaccia con rilassamento:
\[
g^{k+1}=(1-\lambda)g^k+\lambda u_2^{k+1}|_\Gamma,
\qquad 0<\lambda\le 1.
\]
Il metodo e' una iterazione di punto fisso sull'interfaccia. In generale la convergenza non e' automatica per ogni \(\lambda\); per problemi ellittici coercivi si usa un rilassamento adatto, spesso \(\lambda<1\).

**Minimal explanation:**
Un sottodominio riceve un dato di Dirichlet sull'interfaccia. L'altro riceve il flusso di Neumann corrispondente. L'aggiornamento cerca di far coincidere le tracce delle due soluzioni sull'interfaccia.

**Risk check:**
Il segno della condizione di Neumann dipende dalle normali. La frase sicura e':
\[
\alpha\partial_{n_1}u_1+\alpha\partial_{n_2}u_2=0\quad\text{su }\Gamma.
\]

### Exercise 1.5

**What they ask:** Implementare in deal.II il solver FEM con Dirichlet-Neumann rilassato.

**Answer to write:**
Da gestire nella parte pratica: codice/plot/implementazione richiesti.

**Minimal explanation:**
Questo punto chiede codice sorgente del solver, non una derivazione teorica.

**Risk check:**
Sul foglio citare il nome del file sorgente caricato e i dati usati: \(\Omega=(0,3)\times(0,1)\), \(\Omega_1=(0,1)\times(0,1)\), \(\Omega_2=(1,3)\times(0,1)\), \(\alpha=1\), \(\gamma=1\), \(f=1\).

### Exercise 1.6

**What they ask:** Eseguire 15 iterazioni e caricare plot per \(\lambda=1.0\) e \(\lambda=0.25\).

**Answer to write:**
Da gestire nella parte pratica: codice/plot/implementazione richiesti.

**Minimal explanation:**
Questo punto chiede risultati numerici e grafici, non una risposta teorica.

**Risk check:**
Sul foglio citare i nomi dei plot: soluzione finale e profilo su \(y=0.5\), distinguendo \(\lambda=1.0\) e \(\lambda=0.25\).

## Exercise 2

### Exercise 2.1

**What they ask:** Scrivere la formulazione debole del problema di Stokes instazionario.

**Answer to write:**
Definisco
\[
V=\{v\in[H^1(\Omega)]^2: v=0\text{ su }\Gamma_D\},
\qquad
Q=L^2(\Omega)
\]
oppure \(Q=L^2_0(\Omega)\) se serve fissare la pressione. Cercare \(u(t)\in V\), \(p(t)\in Q\), con \(u(0)=0\), tali che per ogni \(v\in V\), \(q\in Q\):
\[
(\partial_t u,v)_\Omega
+\mu(\nabla u,\nabla v)_\Omega
-(p,\operatorname{div}v)_\Omega
=\int_{\Gamma_N}\psi\cdot v\,ds,
\]
\[
(\operatorname{div}u,q)_\Omega=0.
\]

**Minimal explanation:**
La condizione su \(\Gamma_D\) entra nello spazio della velocita'. La condizione di Neumann/traction entra come termine di bordo su \(\Gamma_N\). Il termine di pressione deriva da integrazione per parti di \(\nabla p\).

**Risk check:**
Controllare il segno di \(b(v,p)\). Qui si usa \(b(v,p)=-(p,\operatorname{div}v)\); l'importante e' mantenere lo stesso segno in tutte le righe.

### Exercise 2.2

**What they ask:** Scrivere l'approssimazione spaziale Taylor-Hood con velocita' \(P_3\) e pressione \(P_2\).

**Answer to write:**
Sia \(\mathcal T_h\) una mesh. Uso gli spazi Taylor-Hood:
\[
V_h=\{v_h\in V\cap[C^0(\Omega)]^2: v_h|_K\in[\mathbb P_3(K)]^2\},
\]
\[
Q_h=\{q_h\in Q\cap C^0(\Omega): q_h|_K\in\mathbb P_2(K)\}.
\]
Cercare \(u_h(t)\in V_h\), \(p_h(t)\in Q_h\), con \(u_h(0)=0\), tali che
\[
(\partial_t u_h,v_h)+\mu(\nabla u_h,\nabla v_h)
-(p_h,\operatorname{div}v_h)
=\int_{\Gamma_N}\psi\cdot v_h\,ds,
\]
\[
(\operatorname{div}u_h,q_h)=0
\]
per ogni \(v_h\in V_h\), \(q_h\in Q_h\).

**Minimal explanation:**
Taylor-Hood usa polinomi continui per la velocita' di grado uno piu' alto rispetto alla pressione. La coppia \(P_3/P_2\) e' una scelta standard stabile per Stokes.

**Risk check:**
Se si impone pressione a media nulla, scrivere \(Q_h\subset L^2_0(\Omega)\). Non confondere questo punto con il codice del punto 2.5, che usa \(P_2/P_1\).

### Exercise 2.3

**What they ask:** Applicare backward Euler e spiegare perche' ogni timestep ha soluzione unica.

**Answer to write:**
Sia \(t_n=n\Delta t\). Dato \(u_h^n\), trovare \(u_h^{n+1}\in V_h\), \(p_h^{n+1}\in Q_h\) tali che
\[
\left(\frac{u_h^{n+1}-u_h^n}{\Delta t},v_h\right)
+\mu(\nabla u_h^{n+1},\nabla v_h)
-(p_h^{n+1},\operatorname{div}v_h)
=\int_{\Gamma_N}\psi(t_{n+1})\cdot v_h\,ds,
\]
\[
(\operatorname{div}u_h^{n+1},q_h)=0.
\]
In matrice:
\[
\begin{bmatrix}
M/\Delta t+\mu A & B^T\\
B & 0
\end{bmatrix}
\begin{bmatrix}
U^{n+1}\\ P^{n+1}
\end{bmatrix}
=
\begin{bmatrix}
MU^n/\Delta t+F^{n+1}\\ 0
\end{bmatrix}.
\]
A ogni passo la forma
\[
c(u,v)=\frac1{\Delta t}(u,v)+\mu(\nabla u,\nabla v)
\]
e' coerciva sulla velocita'. La coppia Taylor-Hood soddisfa la condizione inf-sup discreta, quindi il problema di Stokes discreto ha soluzione unica.

**Minimal explanation:**
Backward Euler valuta velocita' e pressione al tempo nuovo \(t_{n+1}\). Il termine di massa \(M/\Delta t\) aiuta la coercivita'. L'inf-sup serve a controllare la pressione.

**Risk check:**
La pressione puo' essere unica solo dopo una normalizzazione, per esempio media nulla, a seconda delle condizioni al bordo.

### Exercise 2.4

**What they ask:** Scrivere la stima di errore attesa in funzione di \(\Delta t\) e \(h\).

**Answer to write:**
Per Taylor-Hood \(P_3/P_2\) e backward Euler, se la soluzione esatta e' abbastanza regolare,
\[
\|u(t_n)-u_h^n\|_{H^1(\Omega)}
+\|p(t_n)-p_h^n\|_{L^2(\Omega)}
\le C(h^3+\Delta t),
\]
e per la velocita' in norma \(L^2\):
\[
\|u(t_n)-u_h^n\|_{L^2(\Omega)}
\le C(h^4+\Delta t).
\]
Quindi il metodo e' di ordine 1 nel tempo, ordine 3 in spazio per velocita' \(H^1\) e pressione \(L^2\), e ordine 4 in spazio per velocita' \(L^2\).

**Minimal explanation:**
Backward Euler produce l'errore temporale \(O(\Delta t)\). Taylor-Hood \(P_k/P_{k-1}\) produce errore spaziale \(O(h^k)\) per velocita' in \(H^1\) e pressione in \(L^2\); qui \(k=3\).

**Risk check:**
Sono stime attese sotto ipotesi di regolarita'. Se si parla del codice del punto 2.5 con \(P_2/P_1\), gli ordini spaziali diventano \(h^2\) in \(H^1/L^2\) pressione e \(h^3\) in \(L^2\) velocita'.

### Exercise 2.5

**What they ask:** Implementare il solver Stokes instazionario in deal.II.

**Answer to write:**
Da gestire nella parte pratica: codice/plot/implementazione richiesti.

**Minimal explanation:**
Questo punto chiede implementazione e caricamento del codice sorgente, non una risposta teorica.

**Risk check:**
Citare il file sorgente caricato. Ricordare che qui il codice usa Taylor-Hood \(P_2/P_1\), non \(P_3/P_2\).

### Exercise 2.6

**What they ask:** Calcolare la soluzione e caricare plot/profili.

**Answer to write:**
Da gestire nella parte pratica: codice/plot/implementazione richiesti.

**Minimal explanation:**
Questo punto richiede post-processing numerico: plot finale, profilo lungo \(y=x\), e plot temporale nel punto richiesto.

**Risk check:**
Sul foglio citare i nomi dei file per: plot a \(T=1\), profilo della velocita' lungo \(R=\Omega\cap\{y=x\}\), e traccia temporale in \(x=(1.25,-0.4)^T\).
