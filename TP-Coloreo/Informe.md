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

Las soluciones generadas por una construcción aleatoria golosa no son necesariamente óptimas.
La fase de búsqueda local generalmente mejora la solución construida. Es decir, recorre iterativamente el vecindario reemplazando sucesivamente la solución actual por una mejor solución en la vecindad de la solución actual.

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


### Reactive GRASP

# Algoritmo

### Heurística Greedy

### Heurística Local Search

### Heurística Greedy Randomized Construction

### Esquema GRASP

### Criterio de parada

### Ordenes de complejidad



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
