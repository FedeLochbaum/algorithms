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

### Criterio de parada

### Ordenes de complejidad

### GRASP + Path relinking

### Reactive GRASP

# Experimentación

### Introducción a las pruebas

### Entorno de prueba

### Tipos de pruebas

### Pruebas

# Conclusiones

### Conclusión de pruebas

### Conclusión de ordenes de complejidad

### Mejoramiento de soluciones

### Conclusión general

### Alternativas de implementación

# Referencias
