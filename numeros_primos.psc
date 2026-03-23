Algoritmo numeros_primos
	Escribir "Ingresa un numero"
	Leer numeroingresado
	iteracion = 1
	divisionresiduocera = 0
	
	Mientras iteracion <= numeroingresado Hacer
		Si numeroingresado MOD iteracion = 0 Entonces
			divisionresiduocera = divisionresiduocera + 1
		FinSi
		iteracion = iteracion + 1
	FinMientras
	
	Si divisionresiduocera = 2 Entonces
		Escribir "El numero es primo"
	SiNo
		Escribir "El numero no es primo"
	FinSi
FinAlgoritmo

