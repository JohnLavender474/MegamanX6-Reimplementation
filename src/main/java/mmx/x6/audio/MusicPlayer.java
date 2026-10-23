package mmx.x6.audio;

public interface MusicPlayer {

    void play(int track);

    void stop();

    boolean playing();

    int track();

    void update();

    MusicPlayer NONE = new MusicPlayer() {
        private int track = -1;

        @Override
        public void play(int t) {
            track = t;
        }

        @Override
        public void stop() {
            track = -1;
        }

        @Override
        public boolean playing() {
            return track >= 0;
        }

        @Override
        public int track() {
            return track;
        }

        @Override
        public void update() {
        }
    };
}
