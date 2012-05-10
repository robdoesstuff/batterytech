//
//  IAPManager.m
//  nscra-ios
//
//  Created by Apple on 11/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "IAPManager.h"
#include <batterytech/batterytech.h>
#include <batterytech/Logger.h>

@implementation IAPManager

+ (IAPManager*)getInstance {
    static IAPManager *instance;
    @synchronized(self) {
        if (!instance) {
            instance = [[IAPManager alloc] init];
        }
        return instance;
    }
}

- (void)requestProductData:(NSString *)sku
{
    NSSet *productIdentifiers = [NSSet setWithObject:sku ];
    productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    productsRequest.delegate = self;
    [productsRequest start];
    
    // we will release the request object in the delegate callback
}

#pragma mark -
#pragma mark SKProductsRequestDelegate methods

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray *products = response.products;
    myProduct = [products count] == 1 ? [[products objectAtIndex:0] retain] : nil;
    if (myProduct)
    {
        NSLog(@"Product title: %@" , myProduct.localizedTitle);
        NSLog(@"Product description: %@" , myProduct.localizedDescription);
        NSLog(@"Product price: %@" , myProduct.price);
        NSLog(@"Product id: %@" , myProduct.productIdentifier);
    }
    
    for (NSString *invalidProductId in response.invalidProductIdentifiers)
    {
        NSLog(@"Invalid product id: %@" , invalidProductId);
    }

    // finally release the reqest we alloc/init’ed in requestProUpgradeProductData
    [productsRequest release];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kIAPManagerProductsFetchedNotification object:self userInfo:nil];
}

//
// call this method once on startup
//
- (void)loadStore
{
    // restarts any purchases if they were interrupted last time the app was open
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }

//
// call this before making a purchase
//
- (BOOL)canMakePurchases
{
    return [SKPaymentQueue canMakePayments];
}

//
// kick off the upgrade transaction
//
- (void)purchaseProduct:(NSString *)sku;
{
    SKPayment *payment = [SKPayment paymentWithProductIdentifier:sku];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

#pragma -
#pragma Purchase helpers

//
// saves a record of the transaction by storing the receipt to disk
//
- (void)recordTransaction:(SKPaymentTransaction *)transaction
{
    //transaction.payment.productIdentifier
}

//
// successful purchase - unlock or provide content
//
- (void)provideContent:(NSString *)productId
{
    NSString *callbackStr = [NSString stringWithFormat:@"purchaseSucceeded %@", productId];
    btCallback([callbackStr UTF8String]);
}

//
// removes the transaction from the queue and posts a notification with the transaction result
//
- (void)finishTransaction:(SKPaymentTransaction *)transaction wasSuccessful:(BOOL)wasSuccessful
{
    // remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    
    NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:transaction, @"transaction" , nil];
    if (wasSuccessful) {
        // send out a notification that we’ve finished the transaction
        [[NSNotificationCenter defaultCenter] postNotificationName:kIAPManagerTransactionSucceededNotification object:self userInfo:userInfo];
    } else {
        // send out a notification for the failed transaction
        [[NSNotificationCenter defaultCenter] postNotificationName:kIAPManagerTransactionFailedNotification object:self userInfo:userInfo];
        NSString *callbackStr = [NSString stringWithFormat:@"purchaseFailed %s", transaction.originalTransaction.payment.productIdentifier];
        btCallback([callbackStr UTF8String]);
    }
}

//
// called when the transaction was successful
//
- (void)completeTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction];
    [self provideContent:transaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

//
// called when a transaction has been restored and and successfully completed
//
- (void)restoreTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction.originalTransaction];
    [self provideContent:transaction.originalTransaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

//
// called when a transaction has failed
//
- (void)failedTransaction:(SKPaymentTransaction *)transaction
{
    switch (transaction.error.code) {
        char buf[1000];
        case SKErrorUnknown:
            logmsg("Transaction Error - Unknown");
            sprintf(buf, "Transaction Error - Error code %d", transaction.error.code);
            logmsg(buf);
            NSLog(@"%@", [transaction.error localizedDescription]);
            NSLog(@"%@", [transaction.error localizedFailureReason]);
            NSLog(@"%@", [transaction.error localizedRecoveryOptions]);
            NSLog(@"%@", [transaction.error localizedRecoverySuggestion]);
           break;
        case SKErrorClientInvalid:
            logmsg("Transaction Error - Client Invalid");
            break;
        case SKErrorPaymentCancelled:
            logmsg("Transaction Error - Payment Cancelled");
           break;
        case SKErrorPaymentInvalid:
            logmsg("Transaction Error - Payment Invalid");
           break;
        case SKErrorPaymentNotAllowed:
            logmsg("Transaction Error - Payment Not Allowed");
           break;
        default:
            sprintf(buf, "Transaction Error - Error code %d", transaction.error.code);
            logmsg(buf);
            NSLog(@"%@", [transaction.error localizedDescription]);
            NSLog(@"%@", [transaction.error localizedFailureReason]);
            NSLog(@"%@", [transaction.error localizedRecoveryOptions]);
            NSLog(@"%@", [transaction.error localizedRecoverySuggestion]);
            break;
    }
    if (transaction.error.code != SKErrorPaymentCancelled) {
        [self finishTransaction:transaction wasSuccessful:NO];
    } else {
        // this is fine, the user just cancelled, so don’t notify
        [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    }
}

#pragma mark -
#pragma mark SKPaymentTransactionObserver methods

//
// called when the transaction status is updated
//
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchasing:
                logmsg("Transaction State Updated to Purchasing");
                break;
            case SKPaymentTransactionStatePurchased:
                logmsg("Transaction State Updated to Purchased");
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                logmsg("Transaction State Updated to Failed");
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                logmsg("Transaction State Updated to Restored");
                [self restoreTransaction:transaction];
                break;
            default:
                break;
        }
    }
}
@end
