public class BubbleSort extends SortAlgorithm {

	public BubbleSort(int input_array[]) {
		super(input_array);
	}
	
    @Override
    public void sort() {
    	int size = arr.length;
        boolean swapped = false;

        for(int i = 0; i < size - 1; ++i){ // outer loop works once for each element
            swapped = false;
            for(int j = 0; j < size - i - 1; ++j){ // inner loop make comparisons in unsorted side of the array
                comparison_counter++; // increment counter for each comparison
                if(arr[j] > arr[j + 1]){ // compare and swap consecutive elements
                    swap(j, j + 1);
                    swapped = true;
                }
            }
            if(!swapped) // if swapped is false array is sorted so break the loop;
                break;
        }
    }
    
    @Override
    public void print() {
    	System.out.print("Bubble Sort\t=>\t");
    	super.print();
    }
}
