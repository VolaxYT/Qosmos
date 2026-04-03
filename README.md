# Qosmos

Qosmos est un simulateur temps réel des orbitales atomiques des atomes hydrogénoïdes. Il résout analytiquement l'équation de Schrödinger en coordonnées sphériques, génère un nuage de points par échantillonnage Monte Carlo selon la densité de probabilité $|\Psi_{nlm}|^2$, et visualise le tout avec OpenGL.

---

## Sommaire

1. [Modèle physique](#1-modèle-physique)
2. [Résolution de l'équation de Schrödinger](#2-résolution-de-léquation-de-schrödinger)
3. [Séparation des variables](#3-séparation-des-variables)
4. [Partie angulaire](#4-partie-angulaire)
5. [Partie radiale](#5-partie-radiale)
6. [Quantification de l'énergie](#6-quantification-de-lénergie)
7. [Orbitales réelles](#7-orbitales-réelles)
8. [Implémentation C++](#8-implémentation-c)
9. [Échantillonnage Monte Carlo](#9-échantillonnage-monte-carlo)
10. [Rendu OpenGL](#10-rendu-opengl)
11. [Structure du projet](#11-structure-du-projet)
12. [Compilation](#12-compilation)
13. [Contrôles](#13-contrôles)
14. [Galerie des orbitales](#14-galerie-des-orbitales)

---

## 1. Modèle physique

On modélise un électron de masse $m_e$ et de charge $-e$ soumis au potentiel coulombien d'un noyau de charge $+Ze$.

La force électrostatique entre les deux charges est :

$$\vec{F} = -\frac{Ze^2}{4\pi\varepsilon_0 r^2}\vec{u_r}$$

L'énergie potentielle associée s'obtient par $F_r = -dV/dr$, soit :

$$V(r) = -\int F_r \, dr = -\frac{Ze^2}{4\pi\varepsilon_0 r}$$

La convention $V(r \to \infty) = 0$ fixe la constante d'intégration à zéro.

---

## 2. Résolution de l'équation de Schrödinger

L'équation de Schrödinger indépendante du temps est :

$$\quad \left[-\frac{\hbar^2}{2m_e}\nabla^2 + V(r)\right]\Psi = E\Psi$$

En coordonnées sphériques $(r, \theta, \phi)$, le Laplacien s'écrit :

$$\nabla^2 = \frac{1}{r^2}\frac{\partial}{\partial r}\left(r^2\frac{\partial}{\partial r}\right) + \frac{1}{r^2\sin\theta}\frac{\partial}{\partial\theta}\left(\sin\theta\frac{\partial}{\partial\theta}\right) + \frac{1}{r^2\sin^2\theta}\frac{\partial^2}{\partial\phi^2}$$

---

## 3. Séparation des variables

On suppose une solution de la forme :

$$\Psi(r, \theta, \phi) = R(r) \cdot Y(\theta, \phi)$$

En substituant dans l'équation de Schrödinger et en divisant par $RY$, les termes en $r$ et en $(\theta, \phi)$ se séparent. Chacun doit être égal à une constante $\lambda$, ce qui donne deux équations découplées.

**Changement de variable radial.** On pose $u(r) = r \cdot R(r)$, ce qui simplifie le terme radial du Laplacien :

$$\frac{1}{r^2}\frac{d}{dr}\left(r^2\frac{dR}{dr}\right) = \frac{1}{r}\frac{d^2u}{dr^2}$$

---

## 4. Partie angulaire

L'équation angulaire se re-sépare en posant $Y(\theta, \phi) = \Theta(\theta)\Phi(\phi)$.

**Équation en $\phi$:**

$$\frac{d^2\Phi}{d\phi^2} = -m^2\Phi \quad \Rightarrow \quad \Phi_m(\phi) = e^{im\phi}$$

La condition de périodicité $\Phi(\phi + 2\pi) = \Phi(\phi)$ impose $m \in \mathbb{Z}$. C'est l'apparition du **nombre quantique magnétique** $m$.

**Équation en $\theta$:**

Avec le changement de variable $x = \cos\theta$ (d'où $\frac{d}{d\theta} = -\sin\theta\frac{d}{dx}$ et $\frac{d^2}{d\theta^2} = (1-x^2)\frac{d^2}{dx^2} - x\frac{d}{dx}$), l'équation en $\theta$ devient l'équation de Legendre associée :

$$\frac{d}{dx}\left[(1-x^2)\frac{d\Theta}{dx}\right] + \left[\lambda - \frac{m^2}{1-x^2}\right]\Theta = 0$$

Cette équation n'admet des solutions régulières que si :

$$\lambda = l(l+1) \quad \text{et} \quad |m| \leq l$$

C'est l'apparition du **nombre quantique orbital** $l$. Les solutions sont les **polynômes de Legendre associés** $P_l^{m}(\cos\theta)$, calculés par la formule de Rodrigues :

$$P_l(x) = \frac{1}{2^l l!}\frac{d^l}{dx^l}(x^2-1)^l, \qquad P_l^m(x) = (-1)^m(1-x^2)^{m/2}\frac{d^m}{dx^m}P_l(x)$$

Les harmoniques sphériques complètes sont :

$$Y_l^m(\theta,\phi) = (-1)^m \cdot \sqrt{\frac{(2l+1)}{4\pi}\frac{(l-m)!}{(l+m)!}} \cdot P_l^{m}(\cos\theta) \cdot e^{im\phi}$$

---

## 5. Partie radiale

L'équation radiale, avec $\lambda = l(l+1)$ et $u(r) = rR(r)$, est :

$$\frac{d^2u}{dr^2} + \left[\frac{2m_e}{\hbar^2}\left(E + \frac{Ze^2}{4\pi\varepsilon_0 r}\right) - \frac{l(l+1)}{r^2}\right]u = 0$$

Pour les états liés ($E < 0$). On pose :

$$\kappa = \sqrt{\frac{-2m_eE}{\hbar^2}} > 0, \qquad \rho = 2\kappa r$$

L'équation adimensionnée est :

$$\frac{d^2u}{d\rho^2} + \left[-\frac{1}{4} + \frac{\rho_0}{4\rho} - \frac{l(l+1)}{\rho^2}\right]u = 0$$

avec $\rho_0 = \frac{m_e Ze^2}{\pi\varepsilon_0\hbar^2\kappa}$.

**Comportements asymptotiques :**
- $\rho \to \infty$ : $u \sim e^{-\rho/2}$
- $\rho \to 0$ : $u \sim \rho^{l+1}$

On factorise $u(\rho) = \rho^{l+1}e^{-\rho/2}v(\rho)$ et on substitue. L'équation obtenue pour $v$ est l'**équation de Laguerre associée** :

$$\rho\, v'' + \left[2(l+1) - \rho\right]v' + \left[\frac{\rho_0}{4} - (l+1)\right]v = 0$$

Pour que $v$ soit un polynôme (condition de normalisabilité), le coefficient constant doit être un entier positif :

$$\frac{\rho_0}{4} - (l+1) = n_r \geq 0, \quad n_r \in \mathbb{N}$$

On définit le **nombre quantique principal** $n = n_r + l + 1$, ce qui impose $l \leq n-1$.

Les solutions sont les **polynômes de Laguerre associés** $L_{n-l-1}^{2l+1}(\rho)$, calculés par récurrence :

$$L_0^\alpha(x) = 1, \quad L_1^\alpha(x) = 1 + \alpha - x$$
$$L_k^\alpha(x) = \frac{(2k-1+\alpha-x)L_{k-1}^\alpha(x) - (k-1+\alpha)L_{k-2}^\alpha(x)}{k}$$

La partie radiale normalisée est :

$$R_{nl}(r) = \sqrt{\left(\frac{2Z}{n}\right)^3 \frac{(n-l-1)!}{2n(n+l)!}} \cdot e^{-\rho/2} \cdot \rho^l \cdot L_{n-l-1}^{2l+1}(\rho), \qquad \rho = \frac{2Zr}{n a_0}$$

---

## 6. Quantification de l'énergie

La condition $\rho_0 = 4n$ donne :

$$\kappa = \frac{Z}{na_0}, \qquad a_0 = \frac{4\pi\varepsilon_0\hbar^2}{m_e e^2} \approx 0{,}529 \text{ Å}$$

L'énergie quantifiée est :

$$\boxed{E_n = -\frac{Z^2 e^2}{8\pi\varepsilon_0 a_0 n^2} = -Z^2 \frac{13{,}6 \text{ eV}}{n^2}}$$

---

## 7. Orbitales réelles

Les fonctions d'onde $\Psi_{nlm}$ sont complexes pour $m \neq 0$ (à cause du facteur $e^{im\phi}$). Leur densité de probabilité $|\Psi_{nlm}|^2$ est identique pour $+m$ et $-m$ car $|e^{im\phi}|^2 = 1$.

Pour obtenir des orbitales visuellement distinctes, on forme des **combinaisons linéaires réelles** :

$$\Psi_{nl}^{cos} = \frac{1}{\sqrt{2}}\left(\Psi_{nl,-m} + \Psi_{nl,+m}\right)$$

$$\Psi_{nl}^{sin} = \frac{i}{\sqrt{2}}\left(\Psi_{nl,-m} - \Psi_{nl,+m}\right)$$

La densité de probabilité est alors :

$$|\Psi_{nlm}^{\text{réel}}|^2 = R_{nl}^2(r) \cdot \Theta_{lm}^2(\theta) \cdot \begin{cases} 1 & m = 0 \\ 2\cos^2(m\phi) & m > 0 \\ 2\sin^2(|m|\phi) & m < 0 \end{cases}$$

---

## 8. Implémentation C++

Toutes les fonctions mathématiques sont dans `src/quantum.h` dans le namespace `Quantum`. On travaille en **unités atomiques** : $a_0 = 1$, ce qui simplifie les expressions.

### Polynômes de Laguerre associés

```cpp
// Polynôme de Laguerre ; partie radiale de Psi
// Calculé par récurrence de Bonnet pour éviter les instabilités numériques
inline double laguerre(int n, int alpha, double x){
    if(n == 0) return 1.0;
    if(n == 1) return 1.0 + alpha - x;

    double L_prev2 = 1.0;
    double L_prev1 = 1.0 + alpha - x;
    double L_curr  = 0.0;

    for(int k = 2; k <= n; k++){
        L_curr  = ((2*k - 1 + alpha - x) * L_prev1 - (k - 1 + alpha)       * L_prev2) / k;
        L_prev2 = L_prev1;
        L_prev1 = L_curr;
    }

    return L_curr;
}
```

### Polynômes de Legendre associés

```cpp
// Polynôme de Legendre ; partie angulaire de Psi
// Calculé par récurrence sur l, toujours avec m = |m|
inline double legendre(int l, int m, double x){
    m = std::abs(m);

    double Pmm = 1.0;
    double factor = 1.0;
    double sqt = std::sqrt(std::max(0.0, 1.0 - x*x));

    for(int i = 1; i <= m; i++){
        Pmm   *= -factor * sqt;
        factor += 2.0;
    }

    if(l == m) return Pmm;

    double Pmm1 = x * (2*m + 1) * Pmm;
    if(l == m + 1) return Pmm1;

    double Pll = 0.0;
    for(int ll = m + 2; ll <= l; ll++){
        Pll  = ((2*ll - 1) * x * Pmm1 - (ll + m - 1) * Pmm) / (ll - m);
        Pmm  = Pmm1;
        Pmm1 = Pll;
    }
    return Pll;
}
```

### Partie radiale

```cpp
// Partie radiale normalisée
inline double R_nl(int n, int l, double r, int Z = 1){
    double rho = 2.0 * Z * r / n;
    // Coefficient de normalisation
    double N = std::sqrt( std::pow(rho, 3) * factorial(n - l - 1) / (2.0 * n * factorial(n + l)));
    double poly = laguerre(n - l - 1, 2*l + 1, rho);
    return N * std::exp(-rho / 2.0) * std::pow(rho, l) * poly;
}
```

### Densité de probabilité

```cpp
// Densité de probabilité |Psi_nlm|² pour les orbitales réelles
// Pour m != 0, on utilise les combinaisons cos / sin qui brisent la dégénérescence entre +m et -m
inline double psi_squared_real(int n, int l, int m, double r, double theta, double phi, int Z = 1){
    if(r < 1e-10) return 0.0;

    double R  = R_nl(n, l, r, Z);
    int am = std::abs(m);

    // Partie angulaire
    double norm = (2*l + 1) / (4.0 * M_PI) * factorial(l - am) / factorial(l + am);
    double P = legendre(l, am, std::cos(theta));
    double Theta = std::sqrt(norm) * P;

    // Partie azimutale
    double angular;
    if(m == 0) angular = Theta;
    else if(m > 0) angular = Theta * std::sqrt(2.0) * std::cos(am * phi);
    else angular = Theta * std::sqrt(2.0) * std::sin(am * phi);

    return R * R * angular * angular;
}
```

---

## 9. Échantillonnage Monte Carlo

La densité de probabilité $|\Psi|^2$ est échantillonnée par **rejection sampling** en coordonnées sphériques.

**Algorithme :**

1. Chercher $p_{\max}$ sur une grille 3D $(r, \theta, \phi)$
2. Tirer $(r, \theta, \phi)$ uniformément dans $[0, r_{\max}] \times [0, \pi] \times [0, 2\pi]$
3. Calculer $p = |\Psi|^2 \cdot r^2 \sin\theta$
4. Accepter avec probabilité $p / p_{\max}$
5. Convertir en cartésien et normaliser par $r_{\max}$

```cpp
double r = rmax * dist01(rng);
double theta = M_PI  * dist01(rng);
double phi = 2.0 * M_PI * dist01(rng);

double p = Quantum::psi_squared_real(n, l, m, r, theta, phi, Z) * r * r * std::sin(theta);

if(dist01(rng) > p / p_max) 
    continue;

float scale = (float)rmax;
float x = (float)(r * std::sin(theta) * std::cos(phi)) / scale;
float y = (float)(r * std::sin(theta) * std::sin(phi)) / scale;
float z = (float)(r * std::cos(theta)) / scale;
```

La normalisation par $r_{\max}$ garantit que toutes les orbitales occupent la même taille à l'écran, quelle que soit leur extension physique.

---

## 10. Rendu OpenGL

### Pipeline

Le rendu utilise `GL_POINTS` avec trois attributs par vertex : position (vec3), couleur RGB (vec3), alpha (float).

```
CPU (sampler)  →  VBO (positions, couleurs, alphas)  →  vertex shader  →  fragment shader  →  écran
```

### Vertex shader

```glsl
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aAlpha;

out vec3 fragColor;
out float fragAlpha;

uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * vec4(aPos, 1.0);

    float dist = length((view * vec4(aPos, 1.0)).xyz);
    float size_by_density = mix(10.0, 3.0, aAlpha * 6.0);
    float size_by_dist = 100.0 / dist;
    gl_PointSize = clamp(size_by_density * size_by_dist, 2.0, 15.0);

    fragColor = aColor;
    fragAlpha = aAlpha;
}
```

La taille des points varie avec la distance à la caméra et avec la densité locale — les zones denses ont des points plus petits, les zones rares des points plus grands, ce qui donne un rendu homogène.

### Fragment shader

```glsl
void main(){
    vec2 coord = gl_PointCoord - vec2(0.5);
    float d = length(coord);
    float falloff = exp(-d * d * 1.5);

    if(d > 0.5) discard;

    screenColor = vec4(falloff * fragColor, fragAlpha);
}
```

`gl_PointCoord` est défini uniquement pour les primitives `GL_POINTS`. Le test circulaire coupe les coins du carré rasterisé. Le falloff gaussien adoucit les bords de chaque point.

### Blend additif

```cpp
glBlendFunc(GL_SRC_ALPHA, GL_ONE);
```

$$\text{couleur finale} = \text{couleur point} \times \alpha + \text{couleur écran}$$

Les zones où beaucoup de points se superposent deviennent plus brillantes, ce qui visualise naturellement les zones de haute densité.

### Palette inferno

Les points sont colorés selon $|\Psi|^2$ normalisé avec la palette inferno (noir → violet → rouge → orange → jaune → blanc) :

```cpp
inline glm::vec3 inferno(float t){
    const glm::vec3 c0(0.000f, 0.000f, 0.000f); // noir
    const glm::vec3 c1(0.180f, 0.020f, 0.320f); // violet foncé
    const glm::vec3 c2(0.560f, 0.050f, 0.380f); // magenta
    const glm::vec3 c3(0.870f, 0.220f, 0.100f); // rouge-orange
    const glm::vec3 c4(0.980f, 0.640f, 0.040f); // jaune-orange
    const glm::vec3 c5(1.000f, 1.000f, 1.000f); // blanc
}
```

---

## 11. Structure du projet

```
qosmos/
├── CMakeLists.txt
├── src/
│   ├── main.cpp          — boucle principale, gestion clavier, rendu
│   ├── config.h          — includes globaux (OpenGL, GLM, STL)
│   ├── camera.h          — caméra orbite en coordonnées sphériques
│   ├── axes.h            — rendu des 3 axes cartésiens
│   ├── point_cloud.h     — gestion GPU du nuage de points (VBOs)
│   ├── quantum.h         — fonctions mathématiques (Laguerre, Legendre, Rnl, psi²)
│   ├── sampler.h         — génération Monte Carlo du nuage
│   ├── glad.c            — loader OpenGL
│   └── shaders/
│       ├── vertex.glsl         — shader points
│       ├── fragment.glsl       — shader points
│       ├── vertex_axes.glsl    — shader axes
│       └── fragment_axes.glsl  — shader axes
└── dependencies/
    ├── glad/glad.h
    ├── KHR/khrplatform.h
    └── stb_image.h
```

---

## 12. Compilation

**Prérequis (Windows + MSYS2/MinGW) :**

```bash
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-glfw
pacman -S mingw-w64-ucrt-x86_64-glm
```

**Build :**

```bash
cmake -B build
cmake --build build
./build/qosmos.exe
```

---

## 13. Contrôles

| Touche | Action |
|---|---|
| Clic gauche + drag | Rotation de la caméra |
| Molette | Zoom |
| R | Rotation automatique on/off |
| Flèche haut / bas | $n + 1$ / $n - 1$ |
| Flèche droite / gauche | $l + 1$ / $l - 1$ |
| U / D | $m + 1$ / $m - 1$ |
| Échap | Quitter |

Le titre de la fenêtre affiche en permanence l'orbitale courante : `Qosmos — n=2 l=1 m=0`.

---

## 14. Galerie des orbitales

### Orbitales s ($l = 0$)

<!-- GIF 1s ici -->
<!-- GIF 2s ici -->
<!-- GIF 3s ici -->

---

### Orbitales p ($l = 1$)

<!-- GIF 2p_z ici -->
<!-- GIF 2p_x ici -->
<!-- GIF 2p_y ici -->

---

### Orbitales d ($l = 2$)

<!-- GIF 3d_z2 ici -->
<!-- GIF 3d_xz ici -->
<!-- GIF 3d_x2y2 ici -->
<!-- GIF 3d_xy ici -->

---

### Orbitales f ($l = 3$)

<!-- GIF 4f ici -->