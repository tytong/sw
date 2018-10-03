// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

package bookstore

import (
	"context"

	api "github.com/pensando/sw/api"
	apiserver "github.com/pensando/sw/venice/apiserver"
	"github.com/pensando/sw/venice/utils/kvstore"
)

// Dummy vars to suppress unused imports message
var _ context.Context
var _ api.ObjectMeta
var _ kvstore.Interface

const KindBook ObjKind = "Book"
const KindCustomer ObjKind = "Customer"
const KindOrder ObjKind = "Order"
const KindPublisher ObjKind = "Publisher"
const KindStore ObjKind = "Store"

// BookstoreV1OrderInterface exposes the CRUD methods for Order
type BookstoreV1OrderInterface interface {
	Create(ctx context.Context, in *Order) (*Order, error)
	Update(ctx context.Context, in *Order) (*Order, error)
	Get(ctx context.Context, objMeta *api.ObjectMeta) (*Order, error)
	Delete(ctx context.Context, objMeta *api.ObjectMeta) (*Order, error)
	List(ctx context.Context, options *api.ListWatchOptions) ([]*Order, error)
	Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error)
	Allowed(oper apiserver.APIOperType) bool
	Applydiscount(ctx context.Context, in *ApplyDiscountReq) (*Order, error)
	Cleardiscount(ctx context.Context, in *ApplyDiscountReq) (*Order, error)
}

// BookstoreV1BookInterface exposes the CRUD methods for Book
type BookstoreV1BookInterface interface {
	Create(ctx context.Context, in *Book) (*Book, error)
	Update(ctx context.Context, in *Book) (*Book, error)
	Get(ctx context.Context, objMeta *api.ObjectMeta) (*Book, error)
	Delete(ctx context.Context, objMeta *api.ObjectMeta) (*Book, error)
	List(ctx context.Context, options *api.ListWatchOptions) ([]*Book, error)
	Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error)
	Allowed(oper apiserver.APIOperType) bool
	Restock(ctx context.Context, in *RestockRequest) (*RestockResponse, error)
}

// BookstoreV1PublisherInterface exposes the CRUD methods for Publisher
type BookstoreV1PublisherInterface interface {
	Create(ctx context.Context, in *Publisher) (*Publisher, error)
	Update(ctx context.Context, in *Publisher) (*Publisher, error)
	Get(ctx context.Context, objMeta *api.ObjectMeta) (*Publisher, error)
	Delete(ctx context.Context, objMeta *api.ObjectMeta) (*Publisher, error)
	List(ctx context.Context, options *api.ListWatchOptions) ([]*Publisher, error)
	Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error)
	Allowed(oper apiserver.APIOperType) bool
}

// BookstoreV1StoreInterface exposes the CRUD methods for Store
type BookstoreV1StoreInterface interface {
	Create(ctx context.Context, in *Store) (*Store, error)
	Update(ctx context.Context, in *Store) (*Store, error)
	Get(ctx context.Context, objMeta *api.ObjectMeta) (*Store, error)
	Delete(ctx context.Context, objMeta *api.ObjectMeta) (*Store, error)
	List(ctx context.Context, options *api.ListWatchOptions) ([]*Store, error)
	Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error)
	Allowed(oper apiserver.APIOperType) bool
	AddOutage(ctx context.Context, in *OutageRequest) (*Store, error)
}

// BookstoreV1CouponInterface exposes the CRUD methods for Coupon
type BookstoreV1CouponInterface interface {
	Create(ctx context.Context, in *Coupon) (*Coupon, error)
	Update(ctx context.Context, in *Coupon) (*Coupon, error)
	Get(ctx context.Context, objMeta *api.ObjectMeta) (*Coupon, error)
	Delete(ctx context.Context, objMeta *api.ObjectMeta) (*Coupon, error)
	List(ctx context.Context, options *api.ListWatchOptions) ([]*Coupon, error)
	Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error)
	Allowed(oper apiserver.APIOperType) bool
}

// BookstoreV1CustomerInterface exposes the CRUD methods for Customer
type BookstoreV1CustomerInterface interface {
	Create(ctx context.Context, in *Customer) (*Customer, error)
	Update(ctx context.Context, in *Customer) (*Customer, error)
	Get(ctx context.Context, objMeta *api.ObjectMeta) (*Customer, error)
	Delete(ctx context.Context, objMeta *api.ObjectMeta) (*Customer, error)
	List(ctx context.Context, options *api.ListWatchOptions) ([]*Customer, error)
	Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error)
	Allowed(oper apiserver.APIOperType) bool
}

// BookstoreV1Interface exposes objects with CRUD operations allowed by the service
type BookstoreV1Interface interface {
	Order() BookstoreV1OrderInterface
	Book() BookstoreV1BookInterface
	Publisher() BookstoreV1PublisherInterface
	Store() BookstoreV1StoreInterface
	Coupon() BookstoreV1CouponInterface
	Customer() BookstoreV1CustomerInterface
	Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error)
}
