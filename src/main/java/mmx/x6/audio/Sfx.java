package mmx.x6.audio;

public interface Sfx {

    void play(int bank, int id);

    void stop(int bank, int id);

    Sfx NONE = new Sfx() {
        @Override
        public void play(int bank, int id) {
        }

        @Override
        public void stop(int bank, int id) {
        }
    };
}
