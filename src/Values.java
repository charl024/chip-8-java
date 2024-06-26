public enum Values {
    WIDTH(64), HEIGHT(32), PIXEL_SIZE(15),
    FONT_ADDRESS(0x50),
    START_ADDRESS(0x200);

    private int value;

    Values(int i) {
        this.value = i;
    }

    public int getValue() {
        return value;
    }

    public void setValue(int value) {
        this.value = value;
    }
}
