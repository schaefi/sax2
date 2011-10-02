//======================================
// Class SaXJava
//--------------------------------------
public class SaX_Java {
	private static boolean _initialized = false;
	public static void initialize() {
		if (!_initialized) {
			try {
				System.load("/usr/lib/sax/plugins/SaX.so");
			} catch (UnsatisfiedLinkError e) {
				System.err.println("Native code library failed to load\n" + e);
				System.exit(1);
			}
			_initialized = true;
		}
	}
}
