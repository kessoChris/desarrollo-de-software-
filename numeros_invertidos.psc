Algoritmo numeros_invertidos
	inverso =0
	Escribir "ingrese un numero"
	leer numero
	residuo = numero
	Mientras residuo > 0 Hacer
		inverso= inverso * 10 + residuo mod 10
		residuo = (residuo - residuo mod 10) / 10
	FinMientras
	Escribir "valor del numero inverso", inverso
	Escribir "el numero", numero
FinAlgoritmo