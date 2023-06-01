from pyhealgo import Ga

import numpy as np




def to_binary(value, number_of_bits, lsb=False):
    if lsb:
        return np.flip(np.array(list(np.binary_repr(value, number_of_bits)), dtype=int))
    return np.array(list(np.binary_repr(value, number_of_bits)), dtype=int)


def to_decimal(x):
    x = np.flip(x)
    y = 0
    for i, j in enumerate(x):
        y += j << i
    return y


MAX_ALLOWED_MASS = 200
TOTAL_VALUE = 0


#items = [(np.random.randint(1,50),np.random.randint(1,100)) for _ in range(0,50)]
items = [(47, 72), (9, 2), (14, 11), (40, 12), (17, 19), (37, 64), (4, 6), (29, 90), (17, 33), (43, 93), (36, 53), (32, 44), (29, 33), (22, 61), (35, 89), (8, 93), (25, 56), (41, 39), (44, 4), (9, 11), (36, 96), (34, 68), (2, 8), (43, 82), (42, 54), (29, 74), (5, 35), (43, 45), (4, 28), (25, 62), (30, 32), (34, 71)]


def on_generation_callback(ga_instance):
    print("Generation: ", ga_instance.current_generation)
    print("Best solution {0} Fitness {1}".format(ga_instance.best_solution, ga_instance.best_fitness))
    print("Item configuration {0}".format(items))
    return 0


def fitness_function(solution_idx, solution):
    total_value = 0
    total_weight = 0
    for i in range(0,len(solution)):
        if solution[i] == 1:
            item = items[i]
            total_value+=item[1]

    for i in range(0, len(solution)):
        if solution[i] == 1:
            item = items[i]
            total_weight+=item[0]

    if total_weight < MAX_ALLOWED_MASS:
        return total_value
    return 0





if __name__ == '__main__':
    ga = Ga(
        chromosome_size=len(items),
        gene_low=0,
        gene_high=2,
        population_size=100,
        mutation_rate=0.02,
        crossover_rate=0.6,
        fitness_func=fitness_function,
        number_of_generations=50,
        crossover_type="uniform",
        selection_type="roulette-wheel-selection",
        stop_criteria_saturate=10,
        on_generation_cbk=on_generation_callback,
         #  on_population_init_cbk=population_init_cbk
    )
    solution, fitness = ga.run()
    ga.visualize()

