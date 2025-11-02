import java.util.HashMap;
import java.util.Map;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.Iterator;

public class alphabet {
	private Set<Character> english_alphabet = new LinkedHashSet<Character>();
	private Map<Character, Map<Character, Character>> map = new HashMap<Character,  Map<Character, Character>>();
	
	public alphabet() {
		// do not edit this method
		fill_english_alphabet();
		fill_map();
	}
	
	private void fill_english_alphabet() {
		// do not edit this method
		for(char c : "ABCDEFGHIJKLMNOPQRSTUVWXYZ".toCharArray()) {
		    english_alphabet.add(c);
		}
	}
	
	private void fill_map() {
		// You must use the "english_alphabet" variable in this method, to fill the "map" variable.
		// You can define 1 or 2 iterators to iterate through the set items.
			Iterator<Character> rowIterator = english_alphabet.iterator();
		// The amount of shifting in the alphabet for a cell depends on the distance of that cell's row and column from the first row and column. 
		// Therefore, the shift amount is found by subtracting the character 'A' from the letters representing rows and columns.
		// Of course, since the shifting process will be circular, the mode of the result is calculated according to 26.
			while (rowIterator.hasNext()) {
				char rowChar = rowIterator.next();
				Map<Character, Character> innerMap = new HashMap<>();
				
				Iterator<Character> colIterator = english_alphabet.iterator();
				while (colIterator.hasNext()) {
					char colChar = colIterator.next();
					
					
					int shift = (rowChar - 'A' + colChar - 'A') % 26;
					char shiftedChar = (char) (shift + 'A');
					
					innerMap.put(colChar, shiftedChar);
				}
				map.put(rowChar, innerMap);
			}
		
		
	}

	public void print_map() {
		// do not edit this method
		System.out.println("*** Viegenere Cipher ***\n\n");
		System.out.println("    " + english_alphabet);
		System.out.print("    ------------------------------------------------------------------------------");
		for(Character k: map.keySet()) {
			System.out.print("\n" + k + " | ");
			System.out.print(map.get(k).values());
		}
		System.out.println("\n");
		
	}

	public Map get_map() {
		return map;
	}
}