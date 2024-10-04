package Projeto_2;
// BarberShop.java
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.time.LocalTime;

public class BarberShop {
    private final int numAssentosEspera;
    private final BlockingQueue<Cliente> filaEspera;
    private boolean noMoreClients = false;

    public BarberShop(int numAssentosEspera, int numBarbeiros) {
        // Aumentamos a capacidade para acomodar os Poison Pills
        this.numAssentosEspera = numAssentosEspera;
        this.filaEspera = new LinkedBlockingQueue<>(numAssentosEspera + numBarbeiros);
    }

    // Método chamado pelo cliente para entrar na barbearia
    public boolean entraNaBarbearia(Cliente cliente) {
        // Usa o método atômico para tentar marcar o cliente como atendido
        if (cliente.tentarAtender()) {
            boolean added = filaEspera.offer(cliente);
            if (added) {
                System.out.println("Cliente-" + cliente.getID() + " esperando corte...");
                return true;
            }
        }
        return false;
    }

    // Método chamado pelo barbeiro para iniciar o serviço
    public Cliente startService(int barbeiroID) {
        try {
            Cliente cliente = filaEspera.take();
            if (cliente == Cliente.POISON_PILL) {
                // Sinal para o barbeiro encerrar
                return null;
            }
            return cliente;
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.out.println("Barbeiro-" + barbeiroID + " foi interrompido enquanto esperava por clientes.");
            return null;
        }
    }

    // Método para sinalizar que não haverá mais clientes e adicionar Poison Pills
    public void setNoMoreClients(int numBarbeiros) {
        noMoreClients = true;
        // Adiciona um Poison Pill para cada barbeiro
        for (int i = 0; i < numBarbeiros; i++) {
            boolean added = filaEspera.offer(Cliente.POISON_PILL);
            if (!added) {
                try {
                    filaEspera.put(Cliente.POISON_PILL);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    System.out.println("Falha ao adicionar Poison Pill.");
                }
            }
        }
    }
}
