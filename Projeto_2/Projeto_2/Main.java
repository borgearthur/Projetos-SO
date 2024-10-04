package Projeto_2;

// Main.java
import java.time.LocalTime;

public class Main {
	public static void main(String[] args) {
		// Verifica se os parâmetros foram fornecidos
		if (args.length != 3) {
			System.out.println(
					"Uso: java Main <numero_de_barbeiros> <numero_de_cadeiras_de_espera> <numero_total_de_clientes>");
			System.exit(1);
		}

		int numBarbeiros = 0;
		int numAssentosEspera = 0;
		int numClientes = 0;

		try {
			numBarbeiros = Integer.parseInt(args[0]);
			numAssentosEspera = Integer.parseInt(args[1]);
			numClientes = Integer.parseInt(args[2]);

			if (numBarbeiros <= 0 || numAssentosEspera < 0 || numClientes <= 0) {
				throw new NumberFormatException();
			}
		} catch (NumberFormatException e) {
			System.out.println(
					"Todos os parâmetros devem ser inteiros positivos. O número de cadeiras de espera pode ser zero ou positivo.");
			System.exit(1);
		}

		BarberShop shop = new BarberShop(numAssentosEspera, numBarbeiros);

		// Cria e inicia as threads dos barbeiros
		Thread[] barbeiros = new Thread[numBarbeiros];
		for (int i = 0; i < numBarbeiros; i++) {
			Barbeiro barbeiro = new Barbeiro(shop);
			barbeiros[i] = new Thread(barbeiro, "Barbeiro-" + barbeiro.getID());
			barbeiros[i].start();
		}

		// Cria e inicia as threads dos clientes
		Thread[] clientes = new Thread[numClientes];
		for (int i = 0; i < numClientes; i++) {
			Cliente cliente = new Cliente(shop);
			clientes[i] = new Thread(cliente, "Cliente-" + cliente.getID());
			clientes[i].start();

			try {
				// Intervalo aleatório entre chegadas de clientes (0 a 1000 ms)
				Thread.sleep((long) (Math.random() * 1000));
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
				System.out.println("Simulação de clientes foi interrompida.");
				System.exit(1);
			}
		}

		// Aguarda todos os clientes terminarem
		for (int i = 0; i < numClientes; i++) {
			try {
				clientes[i].join();
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
				System.out.println("Aguardando clientes foi interrompida.");
			}
		}

		// Indica que não haverá mais clientes
		shop.setNoMoreClients(numBarbeiros);

		// Aguarda todos os barbeiros terminarem
		for (int i = 0; i < numBarbeiros; i++) {
			try {
				barbeiros[i].join();
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
				System.out.println("Aguardando barbeiros foi interrompida.");
			}
		}

		// Imprime mensagem de fechamento
		System.out.println("A barbearia fechou! Todos os clientes foram atendidos.");
	}
}
