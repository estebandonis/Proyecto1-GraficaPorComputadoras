# CC2018-SR2
Utilizar SDL para graficar un objeto de un archivo .obj con flat-shading
## Using MacOS
Para ejecutarlo, debemos de tener los frameworks de SDL en el folder de /Library/Frameworks
y debemos de asegurarnos tener la librería de glm, cambiar dirección en CMakeLists.txt

Si hay un error al momento de leer la imagen, sugiero que se coloque el Path completo de la 
imagen al momento de llamar a la función loadOBJ

Y luego ejecutar el siguiente comando en la terminal:
```shell
./run.sh
```

Al ejecutarlo se verá algo así:


![Screen Recording 2023-11-02 at 6 36 50 PM](https://github.com/estebandonis/Proyecto1-GraficaPorComputadoras/assets/77749546/c88b8330-6332-4dd4-8e2a-a071a2dab59b)

