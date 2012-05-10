//
//  IAPManager.h
//  nscra-ios
//
//  Created by Apple on 11/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

#define kIAPManagerProductsFetchedNotification @"kIAPManagerProductsFetchedNotification"
#define kIAPManagerTransactionFailedNotification @"kIAPManagerTransactionFailedNotification"
#define kIAPManagerTransactionSucceededNotification @"kIAPManagerTransactionSucceededNotification"

@interface IAPManager : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver> {
    SKProduct *myProduct;
    SKProductsRequest *productsRequest;
}

- (void)requestProductData:(NSString *)sku;
- (void)loadStore;
- (BOOL)canMakePurchases;
- (void)purchaseProduct:(NSString *)sku;

// gets the singleton instance of the IAPManager
+ (IAPManager*)getInstance;
@end
