# Quick sheet - sample exam try 2

## Teoria da copiare

### 1.1 Weak formulation
\[
V=H^1_0(\Omega),\qquad
a(u,v)=F(v)\quad\forall v\in V.
\]
\[
a(u,v)=\alpha\int_\Omega \nabla u\cdot\nabla v\,dx
+\gamma\int_\Omega uv\,dx,\qquad
F(v)=\int_\Omega fv\,dx.
\]
Dirichlet omogenea: dentro \(H^1_0(\Omega)\).

### 1.2 Lax-Milgram
\[
|a(u,v)|\le C\|u\|_{H^1}\|v\|_{H^1}.
\]
\[
a(v,v)=\alpha\|\nabla v\|^2+\gamma\|v\|^2
\ge c\|v\|^2_{H^1}
\]
per \(\alpha>0\) e Poincare. \(F\) continuo. Quindi esistenza e unicita'.

### 1.3 FEM \(P_2\)
\[
V_h=\{v_h\in H^1_0(\Omega):v_h|_K\in\mathbb P_2(K)\}.
\]
\[
a(u_h,v_h)=F(v_h)\quad\forall v_h\in V_h.
\]
\[
A_{ij}=\alpha\int_\Omega \nabla\varphi_j\cdot\nabla\varphi_i
+\gamma\int_\Omega \varphi_j\varphi_i,\qquad
F_i=\int_\Omega f\varphi_i.
\]
\[
\|u_h\|_{H^1}\le C\|f\|_{L^2},\qquad
\|u-u_h\|_{H^1}\le Ch^2|u|_{H^3},\qquad
\|u-u_h\|_{L^2}\le Ch^3|u|_{H^3}.
\]

### 1.4 Dirichlet-Neumann
Interfaccia \(\Gamma=\partial\Omega_1\cap\partial\Omega_2\). Dato \(g^k\):
\[
u_1^{k+1}=g^k\quad\text{su }\Gamma.
\]
\[
\alpha\partial_{n_2}u_2^{k+1}
=-\alpha\partial_{n_1}u_1^{k+1}\quad\text{su }\Gamma.
\]
\[
g^{k+1}=(1-\lambda)g^k+\lambda u_2^{k+1}|_\Gamma.
\]
Convergenza: non automatica per ogni \(\lambda\); per problemi ellittici coercivi serve rilassamento adatto.

### 2.1 Weak Stokes
\[
V=\{v\in[H^1(\Omega)]^2:v=0\text{ su }\Gamma_D\},\qquad
Q=L^2(\Omega)\text{ o }L^2_0(\Omega).
\]
\[
(\partial_tu,v)+\mu(\nabla u,\nabla v)
-(p,\operatorname{div}v)=\int_{\Gamma_N}\psi\cdot v\,ds.
\]
\[
(\operatorname{div}u,q)=0,\qquad u(0)=0.
\]

### 2.2 Taylor-Hood \(P_3/P_2\)
\[
V_h=[P_3]^2\cap V,\qquad Q_h=P_2\cap Q.
\]
\[
(\partial_tu_h,v_h)+\mu(\nabla u_h,\nabla v_h)
-(p_h,\operatorname{div}v_h)=\int_{\Gamma_N}\psi\cdot v_h\,ds.
\]
\[
(\operatorname{div}u_h,q_h)=0.
\]
Taylor-Hood \(P_3/P_2\) e' inf-sup stabile.

### 2.3 Backward Euler
\[
\left(\frac{u_h^{n+1}-u_h^n}{\Delta t},v_h\right)
+\mu(\nabla u_h^{n+1},\nabla v_h)
-(p_h^{n+1},\operatorname{div}v_h)
=\int_{\Gamma_N}\psi(t_{n+1})\cdot v_h\,ds.
\]
\[
(\operatorname{div}u_h^{n+1},q_h)=0.
\]
\[
\begin{bmatrix}M/\Delta t+\mu A&B^T\\B&0\end{bmatrix}
\begin{bmatrix}U^{n+1}\\P^{n+1}\end{bmatrix}
=
\begin{bmatrix}MU^n/\Delta t+F^{n+1}\\0\end{bmatrix}.
\]
Unicita': coercivita' di \(M/\Delta t+\mu A\) e inf-sup Taylor-Hood.

### 2.4 Error estimate
\[
\|u(t_n)-u_h^n\|_{H^1}
+\|p(t_n)-p_h^n\|_{L^2}
\le C(h^3+\Delta t).
\]
\[
\|u(t_n)-u_h^n\|_{L^2}\le C(h^4+\Delta t).
\]
Backward Euler: ordine 1 in tempo. \(P_3/P_2\): ordine 3 in \(H^1\) velocita' e \(L^2\) pressione.

## Pratica

### 1.5
Da gestire nella parte pratica: codice/plot/implementazione richiesti. Citare sorgente deal.II Dirichlet-Neumann rilassato.

### 1.6
Da gestire nella parte pratica: codice/plot/implementazione richiesti. Citare plot per 15 iterazioni, \(\lambda=1.0\), \(\lambda=0.25\), soluzione finale e profilo \(y=0.5\).

### 2.5
Da gestire nella parte pratica: codice/plot/implementazione richiesti. Citare sorgente Stokes instazionario \(P_2/P_1\).

### 2.6
Da gestire nella parte pratica: codice/plot/implementazione richiesti. Citare plot finale, profilo lungo \(y=x\), traccia temporale in \((1.25,-0.4)^T\).
