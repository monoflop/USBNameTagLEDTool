/*
 * Small tool to convert a simple hex string of the foramt "00 00 00 00"...
 * to a C array
 */

import java.lang.StringBuilder;

public class HexToCArray
{
  public static void main(String[] args)
  {
    if(args.length != 1)
    {
      System.out.println("Usage: HexToCArray [HEXDATA]");
      System.out.println("HEXDATA format: 00 af 0d 44 ...");
      return;
    }

    StringBuilder stringBuilder = new StringBuilder();
    stringBuilder.append("uint8_t array[] = {").append("\n");

    String[] parts = args[0].split(" ");
    for(int i = 0; i < parts.length; i++)
    {

      if(i < parts.length - 1)
      {
        stringBuilder.append("0x").append(parts[i]).append(", ");
      }
      else
      {
        stringBuilder.append("0x").append(parts[i]);
      }
    }

    stringBuilder.append("\n};");

    System.out.println(stringBuilder.toString());
  }
}
