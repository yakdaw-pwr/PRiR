/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gaussgenerator;

/**
 *
 * @author kamajabu
 */
public class GaussGenerator {

    final static boolean debug = false;

    public static void main(String[] args) {
        int minimum = 1;
        int maximum = 5;

        int size = 5;
        int answer;
        int[][] arrayOfVar = new int[size][size];
        int[] arrayOfNum = new int[size];

        System.out.print(String.valueOf(size) + "\n");

        for (int i = 0; i < size; i++) {
            arrayOfNum[i] = minimum + (int) (Math.random() * maximum);
        }

        for (int i = 0; i < size; i++) {
            answer = 0;
            for (int j = 0; j < size; j++) {
                int randomNum = minimum + (int) (Math.random() * maximum);
                arrayOfVar[i][j] = randomNum;

                if (debug) {
                    System.out.print(String.valueOf(arrayOfVar[i][j]) + "*"
                            + arrayOfNum[j] + "\t");
                } else {
                    System.out.print(String.valueOf(arrayOfVar[i][j] + "\t"));
                }

                answer += arrayOfVar[i][j] * arrayOfNum[j];
            }
            System.out.print(answer + "\n");

        }
        System.out.print("\n\n");
        for (int i = 0; i < size; i++) {
            System.out.print(String.valueOf(arrayOfNum[i]) + "\t");
        }

        System.out.print("\n\n");
    }

}
