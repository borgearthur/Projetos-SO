package Projeto_2;
import java.time.LocalTime;

public class Barbeiro extends Pessoa implements Runnable {
    private static int contadorBarbeiros = 0;
    private final BarberShop shop;

    public Barbeiro(BarberShop shop) {
        super(getNextBarbeiroID());
        this.shop = shop;
    }

    // Método sincronizado para gerar IDs únicos
    private static synchronized int getNextBarbeiroID() {
        return ++contadorBarbeiros;
    }

    @Override
    public void run() {
        while (true) {
            Cliente cliente = shop.startService(getID());
            if (cliente == null) {
                // Sinal para encerrar
                break;
            }

            System.out.println("Cliente-" + cliente.getID() + " cortando cabelo com Barbeiro-" + getID());
            try {
                // Simula o tempo de corte de cabelo (1 a 3 segundos)
                long corteTime = 1000 + (long) (Math.random() * 2000); // 1000 a 3000 ms
                Thread.sleep(corteTime);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                System.out.println("Barbeiro-" + getID() + " foi interrompido durante o corte.");
                break;
            }

            // Notifica o Cliente que o corte foi concluído
            synchronized (cliente) {
                cliente.notify();
            }
        }
        System.out.println("Barbeiro-" + getID() + " finalizando seu trabalho e saindo da barbearia.");
    }
}
