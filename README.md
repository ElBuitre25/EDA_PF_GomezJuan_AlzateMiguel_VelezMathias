# EDA_PF — Análisis de Redes de Transporte Reales

**Estructuras de Datos y Algoritmos — Práctica Final Integradora 2026/01**  
Universidad EAFIT

---

## Integrantes del equipo

| Nombres | 
|---|
| *Velez Matihas* |
| *Alzate Miguel* |
| *Gómez Juan* |

---

## Descripción del proyecto

Este proyecto implementa un análisis completo sobre el dataset **roadNet-PA** (red vial de Pennsylvania, SNAP/Stanford), con aproximadamente 1 millón de nodos y 3 millones de aristas dirigidas. Se desarrollaron tres módulos independientes:

- **Módulo A** — Carga y análisis estructural del grafo.
- **Módulo B** — Camino mínimo punto a punto con Dijkstra y BFS.
- **Módulo C** — Análisis del subgrafo inducido, MST y verificación DAG.

**Fuente del dataset:**  
[https://snap.stanford.edu/data/roadNet-PA.html](https://snap.stanford.edu/data/roadNet-PA.html)  
Citación: Leskovec, J. & Krevl, A. (2014). *SNAP Datasets: Stanford Large Network Dataset Collection.* http://snap.stanford.edu/data

---

## Estructura del repositorio

```
EDA_PF_ApellidoNombre1_ApellidoNombre2/
├── README.md
├── utils/
│   ├── preprocess_dataset.py
    ├── roadNet-PA.txt          # Dataset original de SNAP (descomprimido)
│   └── roadNet.txt             # Dataset preprocesado (generado por preprocess_dataset.py)
├── results/
│   ├── analisis_estructural.txt
│   ├── consultas_p2p.csv
│   ├── camino_Q01.txt
│   ├── camino_Q06.txt
│   ├── subgrafo_caminos.txt
│   ├── analisis_subgrafo.txt
│   └── tiempos.csv
├── src/
│   ├── main.cpp
│   ├── graph.hpp / graph.cpp
│   ├── bfs.hpp / bfs.cpp
│   ├── dijkstra.hpp / dijkstra.cpp
│   └── subgraph_module.hpp / subgraph_module.cpp
│   
└── report/
    └── Informe.pdf
```

---

## Requisitos previos

- Compilador con soporte **C++17** (`g++ >= 7` o `clang++ >= 6`).
- Python 3 (solo para el preprocesamiento del dataset).
- Sistema operativo: Linux /  Windows con MinGW o WSL.

---

## Preprocesamiento del dataset

El archivo original `roadNet-PA.txt` contiene líneas de comentario (prefijo `#`) y aristas dirigidas. El script `preprocess_dataset.py` convierte el dataset a un formato no dirigido y sin duplicados, generando `roadNet.txt`.

```bash
# Colocar roadNet-PA.txt en la carpeta utils/ y ejecutar desde src/:
cd src
python3 preprocess_dataset.py
```

El archivo generado `roadNet.txt` tiene el siguiente formato:

```
<num_nodos> <num_aristas>
u1 v1
u2 v2
...
```

donde cada arista aparece una sola vez (se eliminan duplicados y se tratan como no dirigidas).

---

## Compilación

Desde la raíz del repositorio:

```bash
g++ -std=c++17 -O2 -o eda_pf src/main.cpp src/graph.cpp src/bfs.cpp src/dijkstra.cpp src/subgraph_module.cpp
```

> El flag `-O2` es **obligatorio** para obtener tiempos de ejecución realistas.

---

## Ejecución

El ejecutable recibe tres argumentos:

| Flag | Descripción | Default |
|------|-------------|---------|
| `-m` | Módulo a ejecutar: `A`, `B` o `C` | (requerido) |
| `-d` | Ruta al dataset preprocesado | `../utils/roadNet.txt` |
| `-o` | Directorio de salida de resultados | `../results` |

### Módulo A — Análisis estructural

```bash
./eda_pf -m A -d utils/roadNet.txt -o results
```

Salida: `results/analisis_estructural.txt`

### Módulo B — Camino mínimo P2P

```bash
./eda_pf -m B -d utils/roadNet.txt -o results
```

Salidas:
- `results/consultas_p2p.csv` — Tabla completa de las 10 consultas.
- `results/camino_Q01.txt` — Camino completo Q01 (nodo 1 → 500000).
- `results/camino_Q06.txt` — Camino completo Q06 (nodo 1 → 1087562).
- `results/tiempos.csv` — Resumen de tiempos Dijkstra y BFS.

### Módulo C — Subgrafo inducido

```bash
./eda_pf -m C -d utils/roadNet.txt -o results
```

Salidas:
- `results/subgrafo_caminos.txt` — Subgrafo en formato lista de adyacencia.
- `results/analisis_subgrafo.txt` — Estadísticas del subgrafo, MST y verificación DAG.

---

## Reindexación de nodos

Los identificadores originales del dataset **no son consecutivos** (existen huecos en la numeración). Para poder usar arreglos indexados por posición eficientemente, el programa realiza una reindexación interna:

1. Se leen todos los IDs únicos del archivo.
2. Se ordenan y se asigna un índice interno `0, 1, 2, ..., n-1`.
3. Se mantienen dos mapas:
   - `idToIdx`: ID original → índice interno (`std::unordered_map<int,int>`).
   - `idxToId`: índice interno → ID original (`std::vector<int>`).

Todos los resultados exportados (caminos, subgrafo) usan los **IDs originales** del dataset.

> Ejemplo: el nodo con ID original `500000` puede tener índice interno `423817`. El programa traduce automáticamente en ambas direcciones.

---

## Asignación de pesos

El dataset original no incluye pesos. Para simular distancias viales:

- Se asigna a cada arista un peso entero aleatorio entre **1 y 10** con `peso = (rand() % 10) + 1`.
- Se usa `srand(42)` antes del parseo para garantizar **reproducibilidad**.
- El mismo peso se asigna a ambas direcciones de la arista no dirigida.

Todos los equipos que sigan este esquema exacto obtendrán los mismos resultados en el Módulo B.

---

## Consultas P2P — Módulo B

| Consulta | Origen | Destino |
|----------|--------|---------|
| Q01 | 1 | 500000 |
| Q02 | 100 | 1000000 |
| Q03 | 50000 | 750000 |
| Q04 | 200000 | 800000 |
| Q05 | 300000 | 100000 |
| Q06 | 1 | 1087562 |
| Q07 | 500000 | 1 |
| Q08 | 250000 | 600000 |
| Q09 | 10000 | 900000 |
| Q10 | 400000 | 150000 |

Si un nodo no existe en el grafo o los extremos están en componentes distintas, se reporta `dist_dijkstra = INF` y `saltos_bfs = -1`.

---

## Estadísticas del grafo (comparación con SNAP)

| Estadística | Valor SNAP | Valor obtenido |
|-------------|-----------|----------------|
| Número de nodos | 1,088,092 | Ver `analisis_estructural.txt` |
| Aristas no dirigidas | 1,541,898 | Ver `analisis_estructural.txt` |
| Nodos en componente principal | 1,087,562 | Ver `analisis_estructural.txt` |
| Grado promedio | ~2.83 | Ver `analisis_estructural.txt` |
| Diámetro (estimado) | 782 | Ver `analisis_estructural.txt` |

---

## Implementación — Notas técnicas

- **Representación del grafo:** lista de adyacencia (`std::vector<std::vector<int>>`). La matriz de adyacencia requeriría ~4.7 TB para este grafo y no es viable.
- **Dijkstra:** implementado con `std::priority_queue` de pares `(distancia, nodo)` usando `std::greater<>` para obtener un heap mínimo. Complejidad: O((V + E) log V).
- **BFS:** implementado con `std::queue`. Complejidad: O(V + E).
- **MST (Módulo C):** algoritmo de Kruskal con DSU (Union-Find por rango y compresión de caminos).
- **Verificación DAG:** DFS con coloración de tres estados (no visitado / en proceso / terminado).
- **Medición de tiempos:** `std::chrono::high_resolution_clock`, reportado en milisegundos con dos decimales.

---

## Referencias

- Leskovec, J. & Krevl, A. (2014). *SNAP Datasets: Stanford Large Network Dataset Collection.* [http://snap.stanford.edu/data](http://snap.stanford.edu/data)
- 9th DIMACS Implementation Challenge on Shortest Paths. [http://www.dis.uniroma1.it/challenge9/](http://www.dis.uniroma1.it/challenge9/)
- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to Algorithms* (3rd ed.). MIT Press.