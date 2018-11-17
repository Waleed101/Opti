/* 
    -----------------------------------
    Coffee Ordering System
    -----------------------------------

    Created by: Waleed Sawan
    Date: Nov. 8, 2018
    Description: Allow a user to order small, medium, and large coffees through a basic UI on the Particle console. 
    Integerates component-based development, Particle.publish, and String manipulation. 
    
    Change Log:
    
    Date       | Change   
    -------------------------------------------------------------------------
    11/08/2018 | Created preliminary program to accomplish basic requirements
               |
*/

// Declaring all variables to store customers information
int orderNum = 0, coffeesNum = 0;
int orderType[3] = {0, 0, 0};
float total = 0, totalPaid = 0;

// Declaring constants for prices
const float smallCost = 1.75, mediumCost = 2.00, largeCost = 2.25;

// Declaring different possibilities of what the user might enter 
String posb[4][4] = {{"small", "s", "sm", "smallest"}, {"medium", "m", "med", "middle"}, {"large", "l", "lg", "lrg"}, {"dispense", "disp", "d", "out"}};


void setup()
{
   // Initializing textboxs for the user to enter order, payment, and dispense
   Particle.function("Order", order);
   Particle.function("Pay", payment);
   Particle.function("Dispense", dispense);
   nextCust();
}


void loop()
{
   // Nothing to do here
}


// Function to check what they customer has entered into the order textbox
int order(String command) 
{
    if(orderNum < 5) // Ensure the customer hasn't order the maximum of 5
    {
       if(checkContains(command.toLowerCase(), 0)) // Check if they ordered a small
       {
            coffeesNum++; orderType[0]++; total+=smallCost; // Add small to the order
            Particle.publish("Wrote", "Ordered 1 Small!"); // Acknowledge that they ordered a small
       }
       else if(checkContains(command.toLowerCase(), 1)) // Check if they ordered a medium
       {
            coffeesNum++; orderType[1]++; total+=mediumCost; // Add medium to the order
            Particle.publish("Wrote", "Ordered 1 Medium!"); // Acknowledge that they ordered a medium
       }
       else if(checkContains(command.toLowerCase(), 2)) // Check if they ordered a large
       {
            coffeesNum++; orderType[2]++; total+=largeCost; // Add large to the order
            Particle.publish("Wrote", "Ordered 1 Large!"); // Acknowledge that they ordered a large
       }
    }
    else if(total > 0) // Prompt them to pay if they have ordered the maximum
        Particle.publish("Payment", "You have ordered the maximum number of coffees. Your total is $" + String(total));
    else
        Particle.publish("Dispense", "You have paid in full and ordered the maximum number of coffees. Enter anything into Dispense to get your coffee(s) and any change.");
    
    return 1;
    
    /* ADD HTTP CALL TO SEND WHAT HAS BEEN ORDERED*/
    /* ALSO SEND ANY ERROR MESSAGES */
}

// Function to check what they user has entered into the payment textbox
int payment(String paid)
{
    // Convert string that was read to float
    float paidAmn = paid.toFloat();
    
    if(paidAmn < 0.05)
        Particle.publish("Payment", "We don't take anything less than nickels.");
        
    else
    {
        // Adding amount entered to payment total
        totalPaid+=paidAmn;
        total-=paidAmn;
        
        // Determining what message to send the user 
        if(coffeesNum > 0)
        {
            if(total > 0) // If some payment is customer
                Particle.publish("Payment", "Successfully processed $" + String(totalPaid) + ". You still have to pay $" + String(total) + ". " + String(orderNum));
            else if(total < 0) // If their is change left
                Particle.publish("Payment", "Successfully processed $" + String(totalPaid) + ". Your change is " + String(-1*total) + ". Enter anything into Dispense to get change and coffee(s).");
            else
                Particle.publish("Payment", "Successfully processed $" + String(totalPaid) + ". Your change is " + String(-1*total) + ". Enter anything into Dispense to get coffee(s).");
            
        
        }
        else // Ensure that the customer has order something
            Particle.publish("Wrote", "Please order something before paying.");
    }        
    
    /* ADD HTTP CALL TO SEND TOTAL LEFT AND TOTAL PAID */
    /* ALSO SEND ANY ERROR MESSAGES */
}

// Function to check what they user has entered into the dispense textbox
int dispense(String text)
{
    if(total <= 0 & coffeesNum > 0) // Ensure that there is an order and the payment is in full
    {
        Particle.publish("Product", "Enjoy your " + String(coffeesNum) + " coffee(s)!");
        if(total < 0) // Dispense change if needed
            Particle.publish("Payment", "Your change is " + String(-1*total) + ".");
        
        nextCust(); // Reset and get ready for next customer
    }
    else // Ensure that the customer has order something
        Particle.publish("Wrote", "Please order something before paying.");
    
    /* ADD HTTP CALL TO DISPENSE BEVERAGE AND CHANGE*/
    /* ALSO SEND ANY ERROR MESSAGES */
}

// Function to prompt the next customer
void nextCust()
{
   // Reset all the variables for the customer
   orderNum++; orderType[0] = 0; orderType[1] = 0; orderType[2] = 0; total = 0; totalPaid = 0; coffeesNum = 0;
   
   // Prompt the customer with a welcome message and instructions/rules
   Particle.publish("Welcome", "Welcome to Waleed's Cafe! How may we help you?");
   Particle.publish("Order", "Enter small to order a small, medium to order a medium, or large to order a large.");
   Particle.publish("Order", "You may order up to 5 coffees.");
    
    /* ADD HTTP CALL TO ACKNOWLEDGE NEXT CUSTOMER */
    /* ALSO SEND ANY ERROR MESSAGES */
}

// Function to check if the user has entered small, medium, or large
bool checkContains(String command, int type)
{
    for(int i = 0; i < 4; i++) // Cycle through the different orders
        if(posb[type][i] == command)
            return true;
            
    return false;
}
