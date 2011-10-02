using System;

public class SaXTest {
	static void Main() {
		SaXImport desktop = new SaXImport ( 5 ); // SaX.SAX_DESKTOP
		SaXImport card    = new SaXImport ( 4 ); // SaX.SAX_CARD
		SaXImport path    = new SaXImport ( 9 ); // SaX.SAX_PATH

		card.doImport();
		desktop.doImport();
		path.doImport();

		SaXManipulateDesktop manip = new SaXManipulateDesktop (
			desktop,card,path,0
		);
		String name = manip.getMonitorName();
		Console.WriteLine("Monitor name: " + name);

		//String[] lala = manip.getCDBMonitorList();
		//Console.WriteLine("First element: " + lala[0]);
	}
}
