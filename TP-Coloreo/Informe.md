# Algoritmos - Trabajo Práctico: Metaheurísticas

### Integrante: Lochbaum Federico

### Fecha de entrega: 28/06/2018

# Introducción

### Problema

Una empresa de comunicaciones planifica proveer servicio en una región, para lo cual
se instalarán antenas y se le asignará una frecuencia a cada una. Por cuestiones legales y
ambientales, la ubicación de las antenas está definida de antemano. El costo de instalación
de las antenas está subsidiado, pero el uso de las bandas de frecuencias tiene un costo fijo
que depende de cada frecuencia, independientemente de la cantidad de antenas que tengan
asignada cada una de ellas.
Por otro lado, si un par de antenas que se encuentren lo suficientemente cerca tienen
asignada la misma frecuencia, la empresa se verá forzada a brindar un servicio reducido
a algunos de los clientes del área afectada. Esto producirá una reducción en las ganancias
obtenidas que ya se tienen estimadas por cada par de antenas con posible conflicto.

### Características del problema

El problema propuesto se puede ver como una variante de otro problema, conocido como `Coloreo de grafos`. Dentro de la teoría de grafos, se conoce como `Coloreo de grafos` al problema donde, dado de un Grafo `G` y un conjunto de colores `C = {c1, c2, ..., cn}` se busca asignar a cada vértice del Grafo un color de tal forma que se minimice la cantidad de colores utilizados y además, no suceda que dos vértices vecinos posean el mismo color.

El problema actual posee algunas diferencias con respecto a `Coloreo de grafos`. Se define el Grafo `G` donde el conjunto de vértices del grafo son todas las antenas de la empresa de comunicaciones. A su vez, vamos a definir si dos antenas son `vecinas` cuando suceda que, si comparten el uso de una frecuencia, existe una reducción del servicio. Los colores a asignar sera el conjunto de frecuencias `F = {f1, f2, ..., fn}` disponibles para asignar a dichas antenas y a diferencia del problema general, el objetivo no es utilizar la menor cantidad de frecuencias sin que haya disminución de servicio, sino que consisten en minimizar el costo total que tendrá dicha asignación donde, cada frecuencia tiene un costo de utilización, pagado solo una vez al ser utilizada y, para cada par de antenas vecinas existe un costo de penalización por reducción de servicio.

Por lo tanto, definimos el costo de una asignación de frecuencias como:

`Costo =  Costo por uso de frecuencias + Costo por penalizaciones`

### Propuesta

Como se puede observar, si bien se puede hacer uso de la técnica `Backtracking` para la resolución del problema, no es viable en términos de eficiencia. Por lo tanto, se propone implementar una metaheurística iterativa, ya sea `GRASP` o `Tabú Search` donde, partiendo de una solución global S se busca iterativamente encontrar soluciones S' tal que el C(S') < C(S). Es decir, si en la iesima iteración se encuentra una solución S' tal que su costo es menor al mejor encontrado hasta ese momento, S = S'. Este proceso se repetirá hasta que se cumpla el criterio de parada que tenga dicha metaheurística.

Ambas metaheurísticas hacen uso de dos heurísticas. Una heurística constructiva golosa y una heurística de busqueda local. En la fase constructiva o `Greedy construction` se busca generar una solución greedy tal que, bajo algún criterio, en cada subiteracion de la construcción se elija una asignación donde se optimice el criterio utilizado. Por otro lado, en la busqueda local o `Local seach` se busca, definiendo un vecindario, recorrer dicho vecindario de soluciones vecinas a la generada por la fase constructiva, quedándose siempre  con la mejor solución.

Para la resolución de este trabajo se eligió implementar la metaheurística `GRASP` modificando la heurística golosa (Greedy).

# Preliminares

### GRASP
`Greedy Randomize Adaptive Search Procedures` o `GRASP` es un método iterativo en donde cada iteración consiste básicamente en dos fases. Una fase constructiva y una busqueda local. La fase constructiva ira construyendo una solución factible `S'` y la busqueda local sera la encargada de investigar la vecindad de `S'` hasta llevarla a un mínimo local.


### Esquema general
GRASP almacenara una solución global `S` a medida que transcurran las iteraciones del algoritmo. La solución `S` sera la mejor solución encontrada hasta el momento, por lo tanto, cuando se cumpla el criterio de parada de GRASP, el resultado devuelto por dicho algoritmo sera aquel almacenado en `S`. En cada iteración de `GRASP` se construirá una solución greedy `S'` (fase constructiva) la cual sera modificada hasta encontrar un óptimo local (busqueda local). Si el costo del óptimo local de `S'` es menor al costo de `S`, designaremos a `S'` como la nueva solución global.

El criterio de parada puede ser definido en base a una cantidad máxima de iteraciones, una cantidad máxima de tiempo durante el cual el algoritmo estará corriendo, o bien, alguna cota de proximidad a la mejor solución.

Pseudo código de esquema general:

```
Solución GRASP() {
  S = ∅;
  while(! criterio de parada) {
    S' = Construccion greedy aleatoria();
    S' = Busqueda local(S');
    if(Costo(S') < Costo(S)) {
      S = S';
    }
  }
  return S;
}
```

### Greedy Randomized Construction
En una heurística constructiva golosa simple se busca construir la solución eligiendo la mejor asignación posible en cada subiteracion. Esto, hace que las soluciones generadas por dicha heurística sean totalmente deterministas, es decir, si llamamos N veces a tal función con la misma instancia obtendremos N veces la misma solución. Por lo tanto, es necesario tener un factor no determinista cuando ha de elegirse cada asignación para una solución.

La heurística Golosa aleatoria propone la creación de una lista de posibles candidatos restringidos (RCL) formada por los mejores elementos, es decir, las mejores posibles asignaciones cuya incorporación a la solución actual resulta en los menores costos incrementales. Esto ultimo hace que el algoritmo sea Greedy.

El elemento (asignación) a ser incorporado en cada subiteracion sera seleccionado al azar dentro de RCL (aquí el aspecto no determinista). Una vez seleccionado el elemento e incorporado a la solución actual, la lista de candidatos (RCL) sera actualizada y los costos incrementales serán re-evaluados.

Pseudo código de esquema general:

```
Solución Greedy Randomized Construction(Seed) {
  S = ∅;
  C = Conjunto de todas las posibles asignaciones;
  c = Costo incremental de cada asignación dentro de C;
  while(! Solución esta completa(S)) {
    c_min = Obtener menor elemento de C usando c;
    c_max = Obtener mayor elemento de C usando c;
    RCL = Construir RCL en base a C usando como cota superior {c_min + a(c_max - c_min)};
    Elem = Obtener elemento aleatorio de RCL;
    Agregar Elem a S;
    Actualizar C;
    Actualizar c;
  }
  return S;
}
```


### Utilización de cota superior

Como se puede observar, RCL es utilizada para obtener las mejores posibles asignaciones en cada iteración, calculando el costo incremental de cada asignación para la solución actual. Además de obtener las mejores soluciones, se crea una cota superior que filtra posibles soluciones. Para garantizar que esta cota sea manipulable desde afuera, se construye en base a un numero `a` con valor entre `0, 0.1, 0.2, ..., 1`, lo cual hace que, si `a = 0` se vuelve un algoritmo completamente greedy, mientras que con `a = 1` se vuelve una estrategia aleatoria.

### Local Search

Las soluciones generadas por una construcción aleatoria golosa no son necesariamente óptimas.
La fase de búsqueda local generalmente mejora la solución construida. Es decir, recorre iterativamente el vecindario reemplazando sucesivamente la solución actual por una mejor solución en la vecindad de la solución actual.

Pseudo código de esquema general:

```

Solución Local Search(Solution) {
  while(Solution no optima del vecindario) {
    Encontrar s0 vecino de Solution;
    if(Costo(s0) < Costo(Solución)) {
      Solution = s0;
    }
  }
  return Solution;
}

```

La efectividad de un algoritmo de Local search depende de como este definida la estructura de vecindario y como se implemente la busqueda dentro de este. Usualmente cuando se utiliza una metaheurística GRASP donde las soluciones constructivas poseen buenos resultados, suelen definirse vecindarios simplificados.

### Reactive GRASP

`Reactive GRASP ` es una variante de GRASP que intenta mejorar las soluciones no fijando el valor de `a` en cambio, propone auto-ajustarlo periódicamente a medida que se generen nuevas soluciones.

Se define un conjunto ` A = {0.0, 0.1, ... , 0.9, 1.0}` de posibles valores para `a`, los cuales tendrán una probabilidad de selección ajustada dinámicamente. En la primer iteración, cada elemento de `A` tendrá probabilidad = 1. Para Cada elemento `a_i` de `A` se guardaran todos los valor de las soluciones generadas utilizando `a = a_i`, luego de cada iteración se computa `q_1 = Costo(s*) / a_i ` donde s* es la mejor solución encontrada hasta ese punto. Finalmente, se define la nueva probabilidad `p_i` para `a_i` como `p_i = q_i / ∑ con j=1 hasta n  de q_j`.

### Path Relinking

La idea de path relinking es buscar soluciones que compartan atributos con antiguas soluciones con la esperanza de obtener mejores soluciones.

Path relinking construye nuevas soluciones explotando las trayectorias que conectan a las soluciones buenas, empezando con alguna de las soluciones, llamada la solución de iniciación y generando un camino en la vecindad del espacio que lleva a otras soluciones, llamadas las soluciones guías.

Se han propuesto dos posibles aplicaciones de path relinking en combinación con GRASP:

- Utilizar path relinking como un proceso de optimización posterior a todos los pares de soluciones "elite" encontradas con GRASP.  

- Utilizar path relinking como una estrategia de intensificación aplicada a cada solución obtenida después de la fase de búsqueda local.

# Algoritmo
Como solución para el presente problema, se desarrollo una versión general de `GRASP` que designa valores aleatorios equiprobables para `a`, determinando la cota superior para cada iteración del algoritmo. Además, se implementaron dos variantes para dicha metaheurística, `Path relinking`, `Reactive GRASP` y una versión donde se utilizan ambas.

### Estructuras Auxiliares
Para simplificar la legibilidad del código, se  definieron dos estructuras. Por un lado `Frequency` la representación de una frecuencia, la cual contiene su identificador como la iesima frecuencia y su costo en caso de ser usada. Por otro lado, se definió `Solution` estructura que contiene `assigns` un vector de las asignaciones antena -> frecuencia, `used_frequencies` un vector de las frecuencias utilizadas, indicando cuantas antenas usa la iesima frecuencia y finalmente, `cost` el costo actual de la solución.

### Heurística Greedy
Construyendo una nueva Solución `S` donde no existe ninguna asignación, el algoritmo le asigna a cada una de las antenas la frecuencia que posee menor costo. Luego, haciendo uso del orden topológico del grafo, el algoritmo recorre en dicho orden cada antena `n` y, por cada frecuencia `f`, calcula el costo incremental de asignarle a la antena `n` la frecuencia `f`. Si dicho costo es menor a 0, quiere decir que el costo de la solución `S` disminuirá si se aplica dicha asignación, por lo tanto se reemplaza la asignación anterior de `n` por `f`.

Pseudo código de construccion greedy:

```
Solution greedy_solution() {
    S = Nueva solución;
    for (n : orden topológico){
        for(f : frecuencias) {
            if(n no tiene asignación) {
                Asignarle a n la frecuencia f // frecuencias esta ordenada
            } else {
                if(Costo incremental(n, f) < 0) {
                    Asignarle a n la frecuencia f
                }
            }
        }
    }
    return S;
}
```
Como dicho algoritmo da resultados aceptables dentro de los parametros, se lo utiliza como solución inicial en la implementación de `GRASP`.

### Heurística Greedy Randomized Construction
El algoritmo mas importante de la metaheurística `GRASP`, comienza creando una nueva solución `S` vacia. Define un conjunto `possibles ` de todos los pares de asignaciones `(Antena, Frecuencia)` de longitud `N * F` donde `N` es la cantidad de antenas y `F` la cantidad de frecuencias de la instancia.

Luego, por cada iteración del algoritmo y utilizando el conjunto `possibles` define un nuevo conjunto `RCL` de mejores `K` posibles asignaciones, restringida por `K ` un valor parametrizable que permite obtener mas o menos soluciones y `a` un valor entre `[0..1]` utilizado para definir una cota superior.

Cuando `RCL` es calculado en cada subiteracion, se ordena de menor a mayor todas las asignaciones que posee en ese momento el conjunto `possibles`. Para lograr esto se utiliza una `Priority Queue` que mantiene como invariante que, el par de mayor costo se encuentra en el top de la queue. Este invariante es utilizado para reducir el orden computacional en cada subiteracion. Así, una vez se hayan agregado los primeros `K` pares de asignaciones, solo basta recorrer el resto de `possibles` verificando si el costo incremental del actual par es menor al top de la `Priority Queue`, si es así solo basta quitarlo de la queue y agregar el nuevo par.

Posteriormente, se define la cota superior como `c_min + a * (c_max - c_min)` donde `c_min` es el menor costo incremental observado hasta el momento y `c_max` su opuesto. Entonces, antes de finalizar la construcción de `RCL` se eliminan de la `Priority Queue` todos los pares de asignaciones tal que su costo incremental para el estado actual de la solución `S` es mayor al de la cota superior. Esto es de gran importancia ya que diferentes valores de `a` pueden determinar si la construcción de una solución actual es totalmente `Greedy` o totalmente aleatoria.

Una vez calculado `RCL` se obtiene aleatoriamente un elemento `(I, F)` de dicho conjunto tal que dicha asignación se aplicara a la solución actual. Luego de haber realizado esta asignación deberá actualizarse `possibles` eliminando todos los elementos que representen una posible asignación `F'` para la antena `I`. Por lo tanto dicho algoritmo solo hará una iteración por cada una de las antenas reduciendo constantemente la cantidad de elementos del conjunto de posibles asignaciones.

Pseudo código de construcción aleatoria greedy:

```
Solution greedy_randomized_construction(double a) {
   S = Nueva solución;
   possibles = Todos los posbiles par (i, f) tal que i es una antena y f una frecuencia
   while(S no este completa) {
       RCL = Obtener RCL a partir de possbiles tq |RCL| <= K
       Upper bound = c_min + a * (c_max - c_min)
       Eliminar todos los pares que superen Upper bound
       Assign = Obtener algun elemento aleatorio de RCL
       Aplicar Assign a S

       Actualizar possibles
   }

   return  S;
}
```

### Heurística Local Search
Usualmente cuando se implementa `GRASP` la busqueda local es definida en base a un vecindario simplificado, esto es así ya que, como la heurística de construcción aleatoria logra buenas soluciones, definir un vecindario muy complejo haría que el costo computacional crezca, lo que evitaría la realización de mas iteraciones de fases constructivas. En esta implementación, el vecindario va a estar definido por todas las soluciones que difieran de una sola asignación, esto es, dado dos soluciones `s1` y `s2` existe una única antena `i` tal que la `s1(i) != s2(i)`.

Entonces, dada la solución `S` creada en la fase constructiva, por cada antena `i` de la instancia y por cada frecuencia `f` se calcula si, reemplazando la actual asignación `S(i)` por `f` el costo total de `S` disminuye. Si el costo total de la solución `S` mejora, entonces se reemplaza `S(i)` por `f`.

Pseudo código de busqueda local:
```
Solution local_search(Solution S) {
    for(cada antena i) {
      for(cada frecuencia f) {
        if(el costo de {i, f} < 0) {
          Asignarle a i la frecuencia f
        }
      }
    }
    return S;
}
```

### GRASP
Similar al esquema general, en cada iteración el algoritmo posee tres fases, una fase constructiva, una fase de busqueda local y una fase de actualización de óptimo global.

En cada iteración se elije aleatoriamente un nuevo valor para `a` entre `[0..1]`, el cual es pasado como argumento a la fase constructiva, esta fase constructiva generara una nueva solución `S` totalmente diferente a las soluciones generadas en iteraciones anteriores a la cual se le aplica una busqueda local. Entonces, la busqueda local generara una nueva solución `S'` que sera la mejor solución encontrada dentro del vecindario de `S`. Luego, en fase de actualización se chequea si la solución `S'` es una mejor solución al óptimo global, si es así, el nuevo óptimo global sera  `S'`.

Pseudo código de GRASP:

```
Solution grasp() {
    Sg = greedy_solution();
    while(No se cumpla criterio de parada) {
        a = Generar un numero entre [0..1];

        S = greedy_randomized_construction(a);
        S = local_search(S);
        if(Costo(S) < Costo(Sg)) {
            Sg = S;
        }
    }
    return Sg;
}
```

### Criterio de parada
El criterio de parada utilizado para esta implementación esta basado en un `OR` condicional de dos cosas. Por un lado, una cantidad de tiempo limite de ejecución `T` y por otro, una cantidad máxima de iteraciones de `GRASP`, ambos valores parametrizables para futuras pruebas.

### GRASP + Path Relinking
Una de las variantes implementadas para mejorar las soluciones de GRASP es `Path Relinking`. Este algoritmo es incluido dentro de `GRASP` como una segunda mejora a la solución actual. Es decir, cuando la busqueda local es aplicada a la solución creada por la fase constructiva generando una solución `S'` se aplicara path relinking sobre `S'` generando así una tercera solución local `S''` la cual posteriormente se usara como comparación para la fase de actualización. Ahora, cuando la solución `S''` mejore el óptimo global, no solo sera el nuevo óptimo global sino que también se agregara a `elites`.

Definiremos `Z` un nuevo valor parametrizable el cual representa la cantidad máxima de `elites` a mantener.

En cada iteración de `GRASP`, se elegirá aleatoriamente una solución `Se` dentro de las soluciones `elites` la cual se pasara como argumento a `Path Relinking`, además de `S'` solución resultante de la busqueda local.

Path Relinking definirá un nuevo conjunto `differences` de pares de asignación `(i, f)` tales que `f = Se(i) && Se(i) != S(i)`, representación de las diferencias entre ambas soluciones. Ademas, se define `Sm` como la mejor solución entre `{S, Se}` y finalmente se crea una solucion temporal `St` tal que `St = S`.

Se sabe que, como `Se` es parte de `elites` entonces, en general es una buena solución, si se intenta asemejar `S` a `Se` es posible crear una nueva mejor solución `S''`. Por lo tanto, por cada par `(i, f)` dentro de `differences` se le asignara a `St(i)` la frecuencia `f` aplicandole luego una busqueda local. Si la nueva solución generada mejor el costo total de `Sm` entonces `Sm = St`.

Pseudo código de GRASP con Path relinking:
```
Solution grasp_with_path_relinking() {
    Sg = greedy_solution();
    while(No se cumpla criterio de parada) {
        a  = Generar un numero entre [0..1];
        Se = Solucion aleatoria dentro de elites
        S  = greedy_randomized_construction(a);
        S  = local_search(S);
        S  = Path_relinking(S, Se);
        if(Costo(S) < Costo(Sg)) {
            Agregar S a elites
            Sg = S;
        }
    }
    return Sg;
}
```

Pseudo código de Path relinking:
```
Solution path_relinking(Solution S, Solution Se) {
    differences = diferencias entre S y Se
    Sm = mejor entre S y Se
    St = S;

    while(differences no este vacia) {
        Assign = Obtener mejor assign de differences

        Asignar Assign a St
        Aplicar busqueda local sobre St
        if(Costo(St) < Costo(Sm)) {
          Sm = St;
        }
    }
    return Sm;
}
```

### Reactive GRASP
En el punto anterior se explico como, `Path relinking` puede suplir la falta de memorización de soluciones que posee `GRASP`, permitiendo recordar las mejores soluciones encontradas y así utilizarlas como modelos para el acercamiento a posibles mejores soluciones.

`Reactive GRASP` intenta suplir otra de las faltas propias de `GRASP`, en este caso, propone una forma de, probababilis-ticamente determinar que opciones ha de tomar el algoritmo para construir mejores soluciones a medida que transcurren las iteraciones.

Se define el conjunto `possibles_a` como `{0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}` el cual determina todos los posibles valores de `a` para calcular la cota superior de posibles asignaciones en cada fase constructiva. Además, se definen:

- `All costs`: Conjunto que determina para cada `a_i` el conjunto de costos de aquellas soluciones creadas con `a = a_i`

- `Cost_averages`: Conjunto que determina para cada `a_i` el costo promedio de todas las soluciones creadas con `a = a_i`

- `Quality_of_average_costs`: Conjunto que determinar para cada `a_i` la calidad que posee el costo promedio para `a_i` en comparación a la mejor solución encontrada

- `Probabilities`: Conjunto que determina la probabilidad para cada `a_i` de ser seleccionado en la siguiente iteración

A la implementación de GRASP se le agregara un re conteo cada cierto tiempo, de las probabilidades para cada posible valor de `a`. Además, despues de finalizar cada fase constructiva, se adicionara el costo de la nueva solución a `All costs` para su correspondiente `a`. Esto ultimo es necesario para poder llevar la cuenta de las soluciones encontradas con cada diferente valor de a y así poder calcular, tanto el valor promedio, la calidad de la solución como la probabilidad de ser seleccionada en las siguientes iteraciones.

Por lo tanto, cada N iteraciones se aplicara el reconteo de probabilidades, el cual  calculara nuevamente en `Cost_averages` el costo promedio de solución encontrada para cada a, la calidad de solución en `Quality_of_average_costs` comparada con la mejor solución global encontrada hasta ese momento y finalmente, se calculara en `Probabilities` la probabilidad de cada `a_i` dividiendo la calidad de la solución promedio para `a_i` sobre la sumatoria de calidades de funciones para todo el resto de `a_j`.

Posteriormente se utiliza una función discreta de probabilidad que, utilizando `Probabilities` selecciona en cada iteración el valor de `a` correspondiente.

Pseudo código de Reactive GRASP:

```
Solution reactive_grasp() {
    Sg = greedy_solution();
    while(No se cumpla criterio de parada) {
        if(Condicion para reconteo) {
          Re-calcular-Probabilidades(Sg);
        }

        a = Obtener a en base a Probabilities;
        S = greedy_randomized_construction(a);
        S = local_search(S);

        Agregar Cost(S) a All costs para a
        if(Costo(S) < Costo(Sg)) {
            Sg = S;
        }
    }
    return Sg;
}
```

Pseudo código de Re_calculate_probabilities:
```
void re_calculate_probabilities(int Sg) {
  for(i  : Cost_averages){
    Cost_averages[i] = sumatoria de All_cost[i] / All_cost[i].size();
  }

  for(i : Quality_of_average_costs) {
    Quality_of_average_costs[i] = Costo(Sg) / Cost_averages[i];
  }

  for(i : Probabilities) {
    Probabilities[i] = Quality_of_average_costs[i] / Sumatoria de Quality_of_average_costs;
  }
}
```
### Ordenes de complejidad
A continuación se listan los ordenes de complejidad de cada uno de los algoritmos:

Sea `n` = Cantidad de antenas de la instancia.

Sea `f` = Cantidad de frecuencias de la instancia.

Sea `as` = Cantidad de posibles valores de a.

Sea `j` = Cantidad de máxima soluciones encontradas hasta el momento para un valor de a.

- `Heurística Greedy: O( n * f )`


- `Randomized Greedy: O( n^2 * f )`


- `Local Search: O( n * f )`


- `Path Relinking: O( n^2 * f )`


- `Recalculate Probabilities: O ( as * j ) = O( j )` Dado que `as` es un valor fijo.


# Experimentación

### Introducción a las pruebas
Las pruebas se realizaran sobre 8 instancias provistas para este fin. Se evaluaran diferentes resultados manipulando los diferentes tipos de configuraciones, ya sea, maximizando o minimizando la cantidad de posibles asignaciones a seleccionar en cada iteración (determinado por `K`), la cantidad de tiempo o cantidad de iteraciones limites para la ejecución del algoritmo, diferentes dimensiones para `elites` en caso de `Path relinking` o, el numero necesario de iteraciones para recalcular las probabilidades en caso de `Reactive GRASP`.

Se intentara deducir que factores permiten aproximar el algoritmo a mejores soluciones, hallando los mejores valores globales para estos parametros. Además, se compararan los resultados encontrados en términos de la cantidad de iteraciones necesarias vs la calidad de dichas soluciones.


### Entorno de prueba

#### Hardware

 - Procesador: 	Intel Core i7-7700HQ

 - Arquitectura: 64 bits

 - Velocidad: 3.8 GHz

 - Memoria: 16GB de memoria RAM

#### Software

  - Sistema Operativo: Microsoft Windows 10

  - IDE: Clion 2018 1.2

### Tipos de pruebas
Las instancias utilizadas para las pruebas seran:

  - `dsj500.1` con 500 antenas 60, frecuencias y 3555 posibles conflictos.

  - `miles250` con 128 antenas 9, frecuencias y 387 posibles conflitos.

  - `miles500` con 128 antenas 75, frecuencias y 1170 posibles conflitos.

  - `miles750` con 128 antenas 35, frecuencias y 2113 posibles conflitos.

  - `miles1000` con 128 antenas 45, frecuencias y 3216 posibles conflitos.

  - `miles1500` con 128 antenas 75, frecuencias y 5198 posibles conflitos.

  - `r250.1` con 250 antenas 20, frecuencias y 867 posibles conflitos.

  - `r1000.1` con 1000 antenas 55, frecuencias y 14378 posibles conflitos.

### Pruebas
Las pruebas inicialmente se haran con un tiempo limite de 300 segundos (5 minutos) probando diferentes configuraciones para: la longitud de `RCL`, la longitud de `elites` o la cantidad de iteraciones para ejecutar el reconteo de probabilidades.

#### Primer serie
La primer serie de pruebas se haran con `K = 4`, `|elites| = 5` y cantidad de iteraciones necesarias para reconteo de probabilidades = 40.

##### Dsj500.1

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |          14          |            477          |
|      GRASP + Path relinking     |          14          |         65              |
|          Reactive GRASP         |           14         |       475               |
| Reactive GRASP + Path relinking |           14           |        65                 |

##### Miles250

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |           407           |         36290                |
|      GRASP + Path relinking     |            407          |     10130                    |
|          Reactive GRASP         |              407        |   35602                      |
| Reactive GRASP + Path relinking |          407            |       9922                  |

##### Miles500

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |         3             |              3820           |
|      GRASP + Path relinking     |           3           |                 585        |
|          Reactive GRASP         |           3           |           3796              |
| Reactive GRASP + Path relinking |            3          |          586               |

##### Miles750

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |           1387           |        5543                 |
|      GRASP + Path relinking     |              1088        |             720            |
|          Reactive GRASP         |                1387      |             5828            |
| Reactive GRASP + Path relinking |              1088        |              741           |

##### Miles1000

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |           506           |          2997               |
|      GRASP + Path relinking     |              433        |        356                 |
|          Reactive GRASP         |          506            |   3086                      |
| Reactive GRASP + Path relinking |             433         |      352                   |

##### Miles1500

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |           492           |           1280              |
|      GRASP + Path relinking     |            453          |           134              |
|          Reactive GRASP         |           492           |         1381                |
| Reactive GRASP + Path relinking |               453       |             131            |

##### R250.1

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |            787          |        4771                 |
|      GRASP + Path relinking     |               689       |        1247                 |
|          Reactive GRASP         |             787         |           4941              |
| Reactive GRASP + Path relinking |          689            |         1241                |

##### R1000.1

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |             23         |         85                |
|      GRASP + Path relinking     |                24      |       8                  |
|          Reactive GRASP         |               23       |       86                  |
| Reactive GRASP + Path relinking |            24          |       9                  |

#### Segunda serie
La segunda serie de pruebas se haran con `K = 2`, `|elites| = 4` y cantidad de iteraciones necesarias para reconteo de probabilidades = 10.

##### Dsj500.1

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |            14          |        471                 |
|      GRASP + Path relinking     |            14          |          67               |
|          Reactive GRASP         |               14       |          455               |
| Reactive GRASP + Path relinking |                 14     |           68              |

##### Miles250

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |           434           |         38179                |
|      GRASP + Path relinking     |            407          |       12010                  |
|          Reactive GRASP         |            434          |         38117                |
| Reactive GRASP + Path relinking |             407         |             11988            |

##### Miles500

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |              5        |          4093               |
|      GRASP + Path relinking     |              3        |         621                |
|          Reactive GRASP         |           5           |           4136              |
| Reactive GRASP + Path relinking |            3          |            638             |

##### Miles750

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |            1380          |          6010               |
|      GRASP + Path relinking     |           `1051`           |        801                 |
|          Reactive GRASP         |           1380           |     6015                    |
| Reactive GRASP + Path relinking |           1051           |   791                      |

##### Miles1000

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |            `501`          |           3100              |
|      GRASP + Path relinking     |               447       |       344                  |
|          Reactive GRASP         |               501       |      2913                   |
| Reactive GRASP + Path relinking |                447      |     351                    |

##### Miles1500

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |         `483`             |          1365               |
|      GRASP + Path relinking     |        `448`              |       140                  |
|          Reactive GRASP         |                      |                         |
| Reactive GRASP + Path relinking |                      |                         |

##### R250.1

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |                      |                         |
|      GRASP + Path relinking     |                      |                         |
|          Reactive GRASP         |                      |                         |
| Reactive GRASP + Path relinking |                      |                         |

##### R1000.1

|            Instancia            | Costo mejor solucion | Cantidad de iteraciones |
|:-------------------------------:|:--------------------:|:-----------------------:|
|              GRASP              |                      |                         |
|      GRASP + Path relinking     |                      |                         |
|          Reactive GRASP         |                      |                         |
| Reactive GRASP + Path relinking |                      |                         |

#### Tercera serie
Como se puede observar en las pruebas anteriores, disminuyendo el valor de K a 2 no se logran demasiadas mejoras en comparacion a utilizar K = 4, contrariamente se hallan mejores solucoines en la Primer fase. Por lo tanto la tercera serie de pruebas se haran con `K = 4`, `|elites| = 4` y cantidad de iteraciones necesarias para reconteo de probabilidades = 10 pero con un tiempo limite de 10 minutos, asi se podra observar si realmente hay diferencias entre `Reactive GRASP` y `GRASP` ya que si bien ambas encuentran mismas soluciones optimas el tiempo necesario para llegar a ella desde `Reactive GRASP` es mucho menor.



# Conclusiones

### Conclusión de pruebas

### Conclusión de ordenes de complejidad

### Mejoramiento de soluciones

### Conclusión general

# Referencias
