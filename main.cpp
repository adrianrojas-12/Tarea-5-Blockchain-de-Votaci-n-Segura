#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <iomanip>
using namespace std;

class Voto {
public:
    string id_votante;
    string opcion_elegida;

    Voto(string id, string opcion) {
        id_votante = id;
        opcion_elegida = opcion;
    }

    string toString() const {
        return id_votante + "->" + opcion_elegida;
    }
};

class Block {
public:
    int indice;
    string previous_hash;
    vector<Voto> votos;
    int nonce;
    string current_hash;

    Block(int idx, string prev_hash, vector<Voto> lista_votos) {
        indice = idx;
        previous_hash = prev_hash;
        votos = lista_votos;
        nonce = 0;
        current_hash = calcularHash();
    }

    string calcularHash() const {
        string contenido = to_string(indice) + previous_hash + to_string(nonce);
        for (const auto& voto : votos) {
            contenido += voto.toString();
        }
        hash<string> hasher;
        size_t hash_generado = hasher(contenido);

        return to_string(hash_generado);
    }
};

class BlockChain {
public:
    vector<Block> cadena;
    BlockChain() {
        vector<Voto> votosGenesis;
        votosGenesis.push_back(Voto("000", "Génesis"));
        Block bloqueGenesis(0, "0", votosGenesis);
        cadena.push_back(bloqueGenesis);
    }
    bool isChainValid() {
        for (size_t i = 1; i < cadena.size(); i++) {
            Block bloqueActual = cadena[i];
            Block bloqueAnterior = cadena[i - 1];
            if (bloqueActual.current_hash != bloqueActual.calcularHash()) {
                cout << "¡ALERTA! Los datos del bloque " << i << " fueron alterados." << endl;
                return false;
            }
            if (bloqueActual.previous_hash != bloqueAnterior.current_hash) {
                cout << "¡ALERTA! El enlace entre el bloque " << i-1 << " y " << i << " está roto." << endl;
                return false;
            }
        }
        return true;
    }
};
void recalcularHash(Block& bloque) {
    bloque.current_hash = bloque.calcularHash();
}

bool cumpleDificultad(const Block& bloque, int dificultad) {
    string h = bloque.current_hash;
    string sufijo(dificultad, '0');
    return h.size() >= (size_t)dificultad &&
           h.substr(h.size() - dificultad) == sufijo;
}

void mineBlock(Block& bloque, int dificultad) {
    bloque.nonce = 0;
    recalcularHash(bloque);

    cout << "  [Minando bloque " << bloque.indice << "] ";
    while (!cumpleDificultad(bloque, dificultad)) {
        bloque.nonce++;
        recalcularHash(bloque);
    }
    cout << "Nonce encontrado: " << bloque.nonce
         << " | Hash: " << bloque.current_hash << "\n";
}

class MesaElectoralObserver {
    public:
    virtual void recibirBloque(const Block& bloque) = 0;
    virtual ~MesaElectoralObserver() = default;
};

class CentroElectoralSubject {
    vector<MesaElectoralObserver*> mesas;
    public:
    void registrarMesa(MesaElectoralObserver* mesa) {
        mesas.push_back(mesa);
    }
    void notificarNuevoBloque(const Block& bloque) {
        cout << endl << "[Red Electoral] Notificado bloque " << bloque.indice << " a " << mesas.size()
        << " mesas" << endl;
        for (auto* mesa : mesas) {
            mesa->recibirBloque(bloque);
        }
    }
};

class MesaElectoral : public MesaElectoralObserver {
    string nombre;
    BlockChain cadenaLocal;
    int dificultad;
    bool validarBloque(const Block& bloque) const {
        if (bloque.current_hash != bloque.calcularHash()) return false;
        if (!cumpleDificultad(bloque, dificultad)) return false;
        if (bloque.previous_hash != cadenaLocal.cadena.back().current_hash) return false;
        return true;
    }
    public:
    MesaElectoral(const string& nombre, int dificultad): nombre(nombre), dificultad(dificultad) {}
    void recibirBloque(const Block& bloque) override {
        cout << " " << nombre << ": ";
        if (validarBloque(bloque)) {
            cadenaLocal.cadena.push_back(bloque);
            cout << "Bloque valido. Cadena actualizada (" << cadenaLocal.cadena.size() << " bloques)."
            << "Votos recibidos: " << bloque.votos.size() << endl;
        } else {
            cout << "Bloque rechazado";
        }
    }
    void mostrarResultados() const {
        cout << endl << "=== Resultados en " << nombre << " ===" << endl;
        int contA = 0, contB = 0;
        for (const auto& b : cadenaLocal.cadena)
            for (const auto& v : b.votos) {
                if (v.opcion_elegida == "A") contA++;
                else if (v.opcion_elegida == "B") contB++;
            }
            cout << " Opcion A: " << contA << endl;
            cout << " Opcion B: " << contB << endl;
            cout << " Bloques en cadena: " << cadenaLocal.cadena.size() << endl;
    }
};

int main() {
    int dificultad = 2;

    cout << "=== INICIANDO SISTEMA ELECTORAL BLOCKCHAIN ===" << endl;

    CentroElectoralSubject redElectoral;

    MesaElectoral mesa1("Mesa Norte", dificultad);
    MesaElectoral mesa2("Mesa Sur", dificultad);
    MesaElectoral mesa3("Mesa Central", dificultad);

    redElectoral.registrarMesa(&mesa1);
    redElectoral.registrarMesa(&mesa2);
    redElectoral.registrarMesa(&mesa3);

    BlockChain genesisHelper;
    string hashAnterior = genesisHelper.cadena.back().current_hash;

    vector<Voto> votosBloque1;
    votosBloque1.push_back(Voto("ID_001", "A"));
    votosBloque1.push_back(Voto("ID_002", "B"));
    votosBloque1.push_back(Voto("ID_003", "A"));

    Block bloque1(1, hashAnterior, votosBloque1);

    cout << "\n--- Empieza el conteo de la primera urna ---" << endl;
    mineBlock(bloque1, dificultad);
    redElectoral.notificarNuevoBloque(bloque1);

    vector<Voto> votosBloque2;
    votosBloque2.push_back(Voto("ID_004", "B"));
    votosBloque2.push_back(Voto("ID_005", "B"));
    votosBloque2.push_back(Voto("ID_006", "A"));

    Block bloque2(2, bloque1.current_hash, votosBloque2);

    cout << "\n--- Empieza el conteo de la segunda urna ---" << endl;
    mineBlock(bloque2, dificultad);
    redElectoral.notificarNuevoBloque(bloque2);

    mesa1.mostrarResultados();
    mesa3.mostrarResultados();

    return 0;

}