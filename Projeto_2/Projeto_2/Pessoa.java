package Projeto_2;
// Pessoa.java
public class Pessoa {
    protected final int id; // Declarada como final e NÃO static

    // Construtor que aceita um ID
    public Pessoa(int id) {
        this.id = id;
    }

    public int getID() {
        return id;
    }
}
