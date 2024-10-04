package Projeto_2;
// Cliente.java
import java.util.concurrent.atomic.AtomicBoolean;
import java.time.LocalTime;

public class Cliente extends Pessoa implements Runnable {
    private static int contadorClientes = 0; // Contador estático para IDs únicos de Clientes
    private final BarberShop shop;
    private final AtomicBoolean atendido = new AtomicBoolean(false); // Garantir operações atômicas

    // Construtor para Clientes normais
    public Cliente(BarberShop shop) {
        super(getNextClienteID());
        this.shop = shop;
    }

    // Construtor privado para o Poison Pill
    private Cliente(int id) {
        super(id);
        this.shop = null;
    }

    // Método sincronizado para gerar IDs únicos
    private static synchronized int getNextClienteID() {
        return ++contadorClientes;
    }

    // POISON_PILL como constante estática
    public static final Cliente POISON_PILL = new Cliente(-1);

    public boolean isAtendido() {
        return atendido.get();
    }

    // Tenta marcar o cliente como atendido de forma atômica
    public boolean tentarAtender() {
        return atendido.compareAndSet(false, true);
    }

    @Override
    public void run() {
        // Se for o Poison Pill, não fazer nada
        if (this == POISON_PILL) {
            return;
        }

        System.out.println("Cliente-" + getID() + " tentando cortar o cabelo.");
        boolean entrou = shop.entraNaBarbearia(this);
        if (!entrou) {
            System.out.println("Cliente-" + getID() + " tentou entrar na barbearia, mas está lotada… indo dar uma voltinha");
        } else {
            // Cliente espera ser atendido
            synchronized (this) {
                try {
                    wait(); // Espera até ser notificado pelo barbeiro
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    System.out.println("Cliente-" + getID() + " foi interrompido enquanto esperava.");
                    return;
                }
            }
            // Após ser notificado, o cliente saiu após o corte
            System.out.println("Cliente-" + getID() + " terminou o corte… saindo da barbearia!");
        }
    }
}
