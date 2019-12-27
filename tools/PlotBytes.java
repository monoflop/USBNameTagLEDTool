/*
 * Small tool to print out a simple hex string of the foramt "00 00 00 00"...
 * in binary.
 */

import java.lang.StringBuilder;

public class PlotBytes
{
  public static void main(String[] args)
  {
    if(args.length != 1)
    {
      System.out.println("Usage: PlotBytes [HEXBYTES]");
      return;
    }

    String[] hexBytes = args[0].split(" ");
    StringBuilder stringBuilder = new StringBuilder();
    for(String str : hexBytes)
    {
      int num = Integer.parseInt(str, 16);
      stringBuilder.append(String.format("%8s", Integer.toBinaryString(num)).replace(' ', '0')).append("\n");
    }


    System.out.println(stringBuilder.toString());
  }
}
