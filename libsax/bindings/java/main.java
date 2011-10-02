

public class main {
	static {
		SaX_Java.initialize();
	}

	public static void main(String argv[]) {
		System.out.println( "Creating some objects:" );
		SaXInit init = new SaXInit();
		System.out.println( "Created SaXinit:" + init );

		SaXImport desktop = new SaXImport ( SaX.SAX_DESKTOP );
		SaXImport card    = new SaXImport ( SaX.SAX_CARD );
		SaXImport path    = new SaXImport ( SaX.SAX_PATH );

		card.doImport();
		desktop.doImport();
		path.doImport();

		SaXManipulateDesktop manip = new SaXManipulateDesktop (
			desktop,card,path,0
		);
		String name = manip.getMonitorName();
		System.out.println( "Monitor name:" + name );

		String[] lala = manip.getCDBMonitorList();
		System.out.println( "First element:" + lala[0] );

		SaXManipulateCard manip2 = new SaXManipulateCard ( card,0 );
		String[] opt = manip2.getCardOptions("i810");
		System.out.println( "First Option:" + opt[0] + "Value:" + opt[1] );
	}
}
