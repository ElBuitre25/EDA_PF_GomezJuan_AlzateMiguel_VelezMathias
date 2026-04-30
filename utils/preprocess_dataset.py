def main():
    # print(process_dataset()) --DEBUG
    # print("----------------") --DEBUG
    create_processed_dataset(process_dataset(), count_nodes_edge(process_dataset()))


def process_dataset():
    # count = 0 --DEBUG
    pre_list = []
    undirected_graph = {}

    with open("roadNet-PA.txt", "r") as file:
        for line in file:
            # if count == 20: --DEBUG
            #   break        --DEBUG

            if line[0].isnumeric():
                format_line = line.replace("\t", " ").replace("\n", "")

                nodes = format_line.split(" ")

                try:
                    pre_list.append(tuple(sorted(map(int, nodes))))
                except ValueError:
                    print("Error en ingreso a tuple list")

            # count += 1 --DEBUG
        pre_list = set(pre_list)
        undirected_graph = sorted(pre_list, key=lambda pre_list: pre_list[0])

        # ---DEBUG
        """
            for i in range(len(ordered_list)):
                print(
                    undirected_graph[i],
                    undirected_graph[i][0],
                )
            print(len(undirected_graph), len(undirected_graph))
        """
        # ---END DEBUG
    return undirected_graph


def count_nodes_edge(undirected_graph):
    number_of_nodes = set()
    for tuple in undirected_graph:
        for n in tuple:
            number_of_nodes.add(n)

    return (len(number_of_nodes), len(undirected_graph))


def create_processed_dataset(undirected_graph, count_nodes_edge):
    with open("roadNet.txt", "w") as curated_file:
        curated_file.write(f"{count_nodes_edge[0]} {count_nodes_edge[1]}\n")
        # count = 0 --DEBUG
        for line in undirected_graph:
            # if count == 20: --DEBUG
            # break --DEBUG
            try:
                curated_file.write(f"{line[0]} {line[1]}\n")
                # print(line[0], line[1]) --DEBUG
            except TypeError:
                print("Error en la escritura")
            except FileNotFoundError:
                print("La carpeta de la ruta de escritura no se encuentra")
            except OSError:
                print("Disco lleno")
            except PermissionError:
                print("No se tienen permisos de escritura")
            # count += 1 DEBUG


main()
