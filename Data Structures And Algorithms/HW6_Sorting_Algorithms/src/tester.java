public class tester {

	public static void main(String[] args) {
		int my_array[] = {8, 7, 6, 5, 4, 3, 2, 1}; // this array will be edited by TA during assessment...
		
		System.out.print("Initial Array: ");
		for(int e: my_array)
			System.out.print(e + " ");
		System.out.println("\n");		
		
		SelectionSort ss = new SelectionSort(my_array);
		ss.sort_and_print();
		BubbleSort bs = new BubbleSort(my_array);
		bs.sort_and_print();
		QuickSort qs = new QuickSort(my_array);
		qs.sort_and_print();
		MergeSort ms = new MergeSort(my_array);
		ms.sort_and_print();
	}
}