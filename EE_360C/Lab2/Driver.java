import java.util.ArrayList;

public class Driver {
    
    public static void main(String[] args) {
      
	if (args.length == 0) {
	    System.err.println("usage: java Driver <filename> -t <list of transmission ranges>");
            System.exit(1);
	}

	ArrayList<Double> transmissionRange = new ArrayList<Double>(0);

	String filename = args[0];
	boolean flag_t = false;
        for (int i=1; i<args.length; i++) {
            if(args[i].equals("-t")) {
		flag_t = true;
            } else if(flag_t == true) {
		transmissionRange.add(Double.parseDouble(args[i]));
	    }
            else {
                System.err.printf("Unknown option: %s\n", args[i]);
		System.exit(1);
            }
        }
        
        Program2 network = new Program2(filename);
        System.out.println("---------------------------");
        System.out.println("Results for the input graph");
        System.out.println("---------------------------");
        System.out.println("");
        for (int r = 0; r < transmissionRange.size(); r++) {
            System.out.println("Transmission Range = " + transmissionRange.get(r) + " meters.");
            System.out.println("");
            network.setTransmissionRange(transmissionRange.get(r));
            network.gpsrAllPairs(false);
            network.dijkstraLatencyAllPairs(false);
            network.dijkstraHopsAllPairs(false);
        }
    }
    
}

